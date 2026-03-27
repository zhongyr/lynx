package dev.lynx.compose.compiler

data class ComposePage(
  val sourceName: String,
  val entryName: String,
  val root: ComposeNode,
)

data class ComposeNode(
  val tag: String,
  val styles: LinkedHashMap<String, String> = linkedMapOf(),
  val children: List<ComposeNode> = emptyList(),
  val rawTextChildren: List<String> = emptyList(),
)
