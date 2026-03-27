package dev.lynx.compose.compiler

import org.jetbrains.kotlin.compiler.plugin.AbstractCliOption
import org.jetbrains.kotlin.compiler.plugin.CliOption
import org.jetbrains.kotlin.compiler.plugin.CommandLineProcessor
import org.jetbrains.kotlin.config.CompilerConfiguration
import org.jetbrains.kotlin.config.CompilerConfigurationKey

class ComposeLynxCommandLineProcessor : CommandLineProcessor {
  override val pluginId: String = PLUGIN_ID

  override val pluginOptions: Collection<AbstractCliOption> = listOf(
    CliOption(
      optionName = OPTION_OUTPUT_DIR,
      valueDescription = "<path>",
      description = "Directory used for generated Lynx artifacts",
      required = false,
      allowMultipleOccurrences = false,
    ),
  )

  override fun processOption(
    option: AbstractCliOption,
    value: String,
    configuration: CompilerConfiguration,
  ) {
    when (option.optionName) {
      OPTION_OUTPUT_DIR -> configuration.put(KEY_OUTPUT_DIR, value)
      else -> error("Unexpected ComposeLynx option: ${option.optionName}")
    }
  }

  companion object {
    const val PLUGIN_ID = "dev.lynx.compose.lynx"
    const val OPTION_OUTPUT_DIR = "outputDir"
    val KEY_OUTPUT_DIR = CompilerConfigurationKey<String>("compose-lynx-output-dir")
  }
}
