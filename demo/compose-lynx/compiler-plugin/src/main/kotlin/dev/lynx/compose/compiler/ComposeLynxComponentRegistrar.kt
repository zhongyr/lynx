package dev.lynx.compose.compiler

import com.intellij.mock.MockProject
import org.jetbrains.kotlin.backend.common.extensions.IrGenerationExtension
import org.jetbrains.kotlin.compiler.plugin.ComponentRegistrar
import org.jetbrains.kotlin.config.CompilerConfiguration

class ComposeLynxComponentRegistrar : ComponentRegistrar {
  override fun registerProjectComponents(
    project: MockProject,
    configuration: CompilerConfiguration,
  ) {
    val outputDir = configuration.get(ComposeLynxCommandLineProcessor.KEY_OUTPUT_DIR)
    IrGenerationExtension.registerExtension(
      project,
      ComposeLynxIrGenerationExtension(outputDir),
    )
  }
}
