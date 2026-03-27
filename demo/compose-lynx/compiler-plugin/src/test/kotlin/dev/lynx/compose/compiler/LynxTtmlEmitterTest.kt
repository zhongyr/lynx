package dev.lynx.compose.compiler

import org.junit.Assert.assertTrue
import org.junit.Test

class LynxTtmlEmitterTest {
  @Test
  fun emitsRowColumnAndBackgroundColor() {
    val page = ComposePage(
      sourceName = "Sample.kt",
      entryName = "DemoPage",
      root = ComposeNode(
        tag = "view",
        styles = linkedMapOf(
          "display" to "flex",
          "flex-direction" to "column",
          "background-color" to "#FFFFFF",
        ),
        children = listOf(
          ComposeNode(
            tag = "view",
            styles = linkedMapOf(
              "display" to "flex",
              "flex-direction" to "row",
            ),
          ),
        ),
      ),
    )

    val mainThread = LynxTtmlEmitter.renderMainThreadSource(page)
    val encode = LynxTtmlEmitter.renderEncodeJson(
      page,
      "global.initBundle=function(){return {};};",
      mainThread,
    )
    assertTrue(encode.contains("\"lepusCode\""))
    assertTrue(encode.contains("renderPage"))
    assertTrue(encode.contains("linear-direction:column"))
    assertTrue(encode.contains("linear-direction:row"))
    assertTrue(encode.contains("background-color:#FFFFFF"))
  }
}
