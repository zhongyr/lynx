package dev.lynx.compose.compiler

import org.jetbrains.kotlin.backend.common.extensions.IrPluginContext
import org.jetbrains.kotlin.ir.IrStatement
import org.jetbrains.kotlin.ir.declarations.IrDeclaration
import org.jetbrains.kotlin.ir.declarations.IrDeclarationContainer
import org.jetbrains.kotlin.ir.declarations.IrFunction
import org.jetbrains.kotlin.ir.declarations.IrFile
import org.jetbrains.kotlin.ir.declarations.IrModuleFragment
import org.jetbrains.kotlin.ir.expressions.IrBlock
import org.jetbrains.kotlin.ir.expressions.IrBlockBody
import org.jetbrains.kotlin.ir.expressions.IrCall
import org.jetbrains.kotlin.ir.expressions.IrConst
import org.jetbrains.kotlin.ir.expressions.IrExpression
import org.jetbrains.kotlin.ir.expressions.IrFunctionExpression
import org.jetbrains.kotlin.ir.expressions.IrGetObjectValue
import org.jetbrains.kotlin.ir.expressions.IrReturn
import org.jetbrains.kotlin.ir.util.fqNameWhenAvailable
import org.jetbrains.kotlin.ir.visitors.IrElementVisitorVoid

private const val COMPOSE_ANNOTATION = "dev.lynx.compose.runtime.Compose"
private const val MODIFIER_OBJECT = "dev.lynx.compose.runtime.Modifier"
private const val ROW_FUNCTION = "dev.lynx.compose.runtime.Row"
private const val COLUMN_FUNCTION = "dev.lynx.compose.runtime.Column"
private const val TEXT_FUNCTION = "dev.lynx.compose.runtime.Text"
private const val BACKGROUND_COLOR_FUNCTION = "dev.lynx.compose.runtime.backgroundColor"

class ComposeIrParser(
  private val pluginContext: IrPluginContext,
) {
  fun findComposePages(moduleFragment: IrModuleFragment): List<ComposePage> {
    return moduleFragment.files
      .flatMap { collectFunctions(it) }
      .mapNotNull { function ->
        val root = extractRootNode(function) ?: return@mapNotNull null
        ComposePage(
          sourceName = function.name.asString() + ".kt",
          entryName = function.name.asString(),
          root = root,
        )
      }
  }

  private fun collectFunctions(file: IrFile): List<IrFunction> {
    return collectFunctions(file.declarations)
  }

  private fun collectFunctions(declarations: List<IrDeclaration>): List<IrFunction> {
    val functions = mutableListOf<IrFunction>()
    declarations.forEach { declaration ->
      when (declaration) {
        is IrFunction -> functions += declaration
      }
      if (declaration is IrDeclarationContainer) {
        functions += collectFunctions(declaration.declarations)
      }
    }
    return functions
  }

  private fun parsePage(function: IrFunction): ComposePage {
    val root = extractRootNode(function)
      ?: error("Compose function ${function.name.asString()} does not contain a supported root call.")
    return ComposePage(
      sourceName = function.name.asString() + ".kt",
      entryName = function.name.asString(),
      root = root,
    )
  }

  private fun extractRootNode(function: IrFunction): ComposeNode? {
    val body = function.body ?: return null
    val statements = (body as? IrBlockBody)?.statements ?: return null
    return statements.asSequence()
      .mapNotNull { findSupportedNodeInStatement(it) }
      .firstOrNull()
  }

  private fun findSupportedNodeInStatement(statement: IrStatement): ComposeNode? {
    parseTopLevelStatement(statement)?.let { return it }

    var found: ComposeNode? = null
    statement.accept(object : IrElementVisitorVoid {
      override fun visitElement(element: org.jetbrains.kotlin.ir.IrElement) {
        if (found == null) {
          element.acceptChildren(this, null)
        }
      }

      override fun visitCall(expression: IrCall) {
        if (found == null) {
          found = parseNode(expression)
        }
        if (found == null) {
          expression.acceptChildren(this, null)
        }
      }
    }, null)
    return found
  }

  private fun parseTopLevelStatement(statement: IrStatement): ComposeNode? {
    return when (statement) {
      is IrCall -> parseNode(statement)
      is IrReturn -> parseNode(statement.value)
      is IrBlock -> statement.statements.asSequence().mapNotNull { parseTopLevelStatement(it) }.firstOrNull()
      else -> null
    }
  }

  private fun parseNode(expression: IrExpression?): ComposeNode? {
    return when (expression) {
      is IrCall -> {
        when (expression.symbol.owner.fqNameWhenAvailable?.asString()) {
          ROW_FUNCTION -> parseContainer(expression, "view", "row")
          COLUMN_FUNCTION -> parseContainer(expression, "view", "column")
          TEXT_FUNCTION -> parseText(expression)
          else -> null
        }
      }
      is IrBlock -> expression.statements.asSequence().mapNotNull { parseTopLevelStatement(it) }.firstOrNull()
      else -> null
    }
  }

  private fun parseContainer(
    call: IrCall,
    tag: String,
    direction: String,
  ): ComposeNode {
    val styles = linkedMapOf(
      "display" to "flex",
      "flex-direction" to direction,
    )
    styles.putAll(parseModifier(call.getValueArgument(0)))

    val children = parseLambda(call.getValueArgument(1))
    return ComposeNode(
      tag = tag,
      styles = styles,
      children = children,
    )
  }

  private fun parseText(call: IrCall): ComposeNode {
    val value = stringConst(call.getValueArgument(0))
      ?: error("Text only supports string literals in this demo.")
    return ComposeNode(
      tag = "text",
      rawTextChildren = listOf(value),
    )
  }

  private fun parseLambda(expression: IrExpression?): List<ComposeNode> {
    val functionExpression = when (expression) {
      is IrFunctionExpression -> expression
      is IrBlock -> expression.statements.filterIsInstance<IrFunctionExpression>().firstOrNull()
      else -> null
    } ?: return emptyList()

    val body = functionExpression.function.body ?: return emptyList()
    val statements = (body as? IrBlockBody)?.statements ?: return emptyList()
    return statements.mapNotNull { parseTopLevelStatement(it) }
  }

  private fun parseModifier(expression: IrExpression?): LinkedHashMap<String, String> {
    return when (expression) {
      null -> linkedMapOf()
      is IrGetObjectValue -> {
        val fqName = expression.symbol.owner.fqNameWhenAvailable?.asString()
        if (fqName == MODIFIER_OBJECT) linkedMapOf() else error("Unsupported modifier object: $fqName")
      }
      is IrCall -> {
        when (expression.symbol.owner.fqNameWhenAvailable?.asString()) {
          BACKGROUND_COLOR_FUNCTION -> {
            val styles = parseModifier(expression.extensionReceiver)
            val color = stringConst(expression.getValueArgument(0))
              ?: error("backgroundColor only supports string literals in this demo.")
            styles["background-color"] = color
            styles
          }
          else -> error("Unsupported modifier call: ${expression.symbol.owner.fqNameWhenAvailable}")
        }
      }
      else -> error("Unsupported modifier expression: ${expression::class.simpleName}")
    }
  }

  private fun stringConst(expression: IrExpression?): String? {
    val const = expression as? IrConst<*> ?: return null
    return const.value as? String
  }
}
