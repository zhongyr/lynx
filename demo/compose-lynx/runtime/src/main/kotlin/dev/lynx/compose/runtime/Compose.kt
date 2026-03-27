package dev.lynx.compose.runtime

@Target(AnnotationTarget.FUNCTION)
@Retention(AnnotationRetention.RUNTIME)
annotation class Compose

object Modifier

fun Modifier.backgroundColor(color: String): Modifier = this

fun Row(
  modifier: Modifier = Modifier,
  content: () -> Unit,
) {
  content()
}

fun Column(
  modifier: Modifier = Modifier,
  content: () -> Unit,
) {
  content()
}

fun Text(value: String) {
  value.length
}
