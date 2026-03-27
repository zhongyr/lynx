package dev.lynx.compose.demo

import dev.lynx.compose.runtime.Column
import dev.lynx.compose.runtime.Compose
import dev.lynx.compose.runtime.Modifier
import dev.lynx.compose.runtime.Row
import dev.lynx.compose.runtime.Text
import dev.lynx.compose.runtime.backgroundColor

@Compose
fun DemoPage() {
  Column(modifier = Modifier.backgroundColor("#F5F7FA")) {
    Row(modifier = Modifier.backgroundColor("#FFD54F")) {
      Text("Row Left")
      Text("Row Right")
    }
    Column(modifier = Modifier.backgroundColor("#80DEEA")) {
      Text("Column Top")
      Text("Column Bottom")
    }
  }
}
