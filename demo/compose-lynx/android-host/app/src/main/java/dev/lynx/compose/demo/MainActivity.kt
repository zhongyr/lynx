package dev.lynx.compose.demo

import android.app.Activity
import android.os.Bundle
import android.widget.FrameLayout
import com.lynx.tasm.LynxLoadMeta
import com.lynx.tasm.LynxView
import com.lynx.tasm.LynxViewBuilder

class MainActivity : Activity() {
  private var lynxView: LynxView? = null

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)

    val container = FrameLayout(this)
    setContentView(container)

    val view = LynxView(this, LynxViewBuilder())
    lynxView = view
    container.addView(
      view,
      FrameLayout.LayoutParams(
        FrameLayout.LayoutParams.MATCH_PARENT,
        FrameLayout.LayoutParams.MATCH_PARENT,
      ),
    )

    val bundleData = assets.open("compose-demo.lynx.bundle").use { it.readBytes() }
    val meta = LynxLoadMeta.Builder().apply {
      setUrl("file://lynx?local://compose-demo.lynx.bundle")
      setBinaryData(bundleData)
    }.build()

    view.loadTemplate(meta)
  }

  override fun onDestroy() {
    lynxView?.destroy()
    super.onDestroy()
  }
}
