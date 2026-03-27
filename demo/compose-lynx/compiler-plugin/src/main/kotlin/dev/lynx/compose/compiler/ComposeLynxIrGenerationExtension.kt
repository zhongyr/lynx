package dev.lynx.compose.compiler

import org.jetbrains.kotlin.backend.common.extensions.IrGenerationExtension
import org.jetbrains.kotlin.backend.common.extensions.IrPluginContext
import org.jetbrains.kotlin.ir.declarations.IrModuleFragment
import java.io.File

class ComposeLynxIrGenerationExtension(
  private val outputDir: String?,
) : IrGenerationExtension {
  override fun generate(
    moduleFragment: IrModuleFragment,
    pluginContext: IrPluginContext,
  ) {
    val targetDir = File(outputDir ?: "build/generated/lynx")
    targetDir.mkdirs()

    val parser = ComposeIrParser(pluginContext)
    val emitter = LynxArtifactEmitter()

    val pages = parser.findComposePages(moduleFragment).ifEmpty {
      listOf(
        ComposePage(
          sourceName = "Fallback.kt",
          entryName = "DemoPage",
          root = ComposeNode(
            tag = "view",
            styles = linkedMapOf(
              "display" to "flex",
              "flex-direction" to "column",
              "background-color" to "#F5F7FA",
            ),
            children = listOf(
              ComposeNode(
                tag = "view",
                styles = linkedMapOf(
                  "display" to "flex",
                  "flex-direction" to "row",
                  "background-color" to "#FFD54F",
                ),
                children = listOf(
                  ComposeNode(tag = "text", rawTextChildren = listOf("Row Left")),
                  ComposeNode(tag = "text", rawTextChildren = listOf("Row Right")),
                ),
              ),
              ComposeNode(
                tag = "view",
                styles = linkedMapOf(
                  "display" to "flex",
                  "flex-direction" to "column",
                  "background-color" to "#80DEEA",
                ),
                children = listOf(
                  ComposeNode(tag = "text", rawTextChildren = listOf("Column Top")),
                  ComposeNode(tag = "text", rawTextChildren = listOf("Column Bottom")),
                ),
              ),
            ),
          ),
        )
      )
    }

    pages.forEach { page ->
      emitter.emit(page, targetDir)
    }
  }
}
