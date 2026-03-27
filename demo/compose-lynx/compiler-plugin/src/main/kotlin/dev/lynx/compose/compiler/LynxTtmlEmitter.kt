package dev.lynx.compose.compiler

object LynxTtmlEmitter {
  fun renderEncodeJson(page: ComposePage, jsSource: String, mainThreadSource: String): String {
    val pagePath = "/" + page.entryName + ".js"
    val appServiceSource = renderAppService(pagePath)
    return buildString {
      append("{\n")
      append("  \"compilerOptions\": {\n")
      append("    \"enableFiberArch\": true,\n")
      append("    \"useLepusNG\": true,\n")
      append("    \"enableReuseContext\": true,\n")
      append("    \"bundleModuleMode\": \"ReturnByFunction\",\n")
      append("    \"templateDebugUrl\": \"\",\n")
      append("    \"debugInfoOutside\": true,\n")
      append("    \"defaultDisplayLinear\": true,\n")
      append("    \"enableCSSInvalidation\": false,\n")
      append("    \"enableCSSSelector\": true,\n")
      append("    \"enableLepusDebug\": false,\n")
      append("    \"enableParallelElement\": false,\n")
      append("    \"enableRemoveCSSScope\": true,\n")
      append("    \"targetSdkVersion\": \"2.10\",\n")
      append("    \"defaultOverflowVisible\": true\n")
      append("  },\n")
      append("  \"manifest\": {\n")
      append("    \"/app-service.js\": ${json(appServiceSource)},\n")
      append("    ${json(pagePath)}: ${json(jsSource)}\n")
      append("  },\n")
      append("  \"sourceContent\": ")
      append(renderSourceContent(page).prependIndent("  ").trimStart())
      append(",\n")
      append("  \"css\": {\n")
      append("    \"cssMap\": {},\n")
      append("    \"cssSource\": {}\n")
      append("  },\n")
      append("  \"lepusCode\": {\n")
      append("    \"root\": ${json(mainThreadSource)},\n")
      append("    \"lepusChunk\": {}\n")
      append("  },\n")
      append("  \"customSections\": {},\n")
      append("  \"elementTemplate\": {}\n")
      append("}\n")
    }
  }

  fun renderSourceContent(page: ComposePage): String {
    return buildString {
      append("{\n")
      append("  \"appType\": \"card\",\n")
      append("  \"dsl\": \"react_nodiff\",\n")
      append("  \"config\": {\n")
      append("    \"lepusStrict\": true,\n")
      append("    \"useNewSwiper\": true,\n")
      append("    \"enableICU\": false,\n")
      append("    \"enableNewIntersectionObserver\": true,\n")
      append("    \"enableNativeList\": false,\n")
      append("    \"enableA11y\": true,\n")
      append("    \"enableAccessibilityElement\": false,\n")
      append("    \"enableCSSInheritance\": false,\n")
      append("    \"enableNewGesture\": false,\n")
      append("    \"pipelineSchedulerConfig\": 65536,\n")
      append("    \"removeDescendantSelectorScope\": true,\n")
      append("    \"enableJsBindingApiThrowException\": true\n")
      append("  },\n")
      append("  \"__version\": {\n")
      append("    \"cli\": \"compose-lynx-demo\",\n")
      append("    \"entryName\": ${json(page.entryName)}\n")
      append("  },\n")
      append("  \"pages\": []\n")
      append("}")
    }
  }

  fun renderMainThreadSource(page: ComposePage): String {
    return renderLepusRoot(page)
  }

  private fun renderAppService(pagePath: String): String {
    return buildString {
      append("(function(){'use strict';")
      append("function n({tt}){")
      append("tt.define('/app-service.js',function(e,module,_,i,l,u,a,c,s,f,p,d,h,v,g,y,lynx){")
      append("module.exports=lynx.requireModule(")
      append(json(pagePath))
      append(",globDynamicComponentEntry?globDynamicComponentEntry:'__Card__')")
      append("});")
      append("return tt.require('/app-service.js');")
      append("}")
      append("return{init:n}})()")
    }
  }

  private fun renderLepusRoot(page: ComposePage): String {
    return buildString {
      append("\"use strict\";var e=e||\"__Card__\";(()=>{")
      append("let __composePage;let __composePageId;")
      append("function __composeBuildPage(){")
      append(renderCreateNode(page.root, "root_0"))
      append("__AppendElement(__composePage,root_0);")
      append("}")
      append("function renderPage(data){")
      append("data=data||{};")
      append("__composePage=__CreatePage(\"0\",0);")
      append("__composePageId=__GetElementUniqueID(__composePage);")
      append("__composeBuildPage();")
      append("__FlushElementTree(__composePage);")
      append("}")
      append("function updatePage(data,options){")
      append("renderPage(data);")
      append("if(options){__FlushElementTree(__composePage,options);}")
      append("}")
      append("function updateGlobalProps(data,options){")
      append("if(options){__FlushElementTree(__composePage,options);}else if(__composePage){__FlushElementTree(__composePage);}")
      append("}")
      append("Object.assign(globalThis,{")
      append("renderPage:renderPage,")
      append("updatePage:updatePage,")
      append("updateGlobalProps:updateGlobalProps,")
      append("getPageData:function(){return null;},")
      append("removeComponents:function(){}")
      append("});")
      append("})();")
    }
  }

  private fun renderCreateNode(node: ComposeNode, variableName: String): String {
    return buildString {
      when (node.tag) {
        "view" -> append("let $variableName=__CreateView(__composePageId);")
        "text" -> append("let $variableName=__CreateText(__composePageId);")
        else -> append("let $variableName=__CreateElement(${json(node.tag)},__composePageId);")
      }

      val style = renderStyle(node.styles)
      if (style.isNotEmpty()) {
        append("__SetInlineStyles($variableName,${json(style)});")
      }

      node.rawTextChildren.forEachIndexed { index, rawText ->
        val rawTextVar = "${variableName}_text_$index"
        append("let $rawTextVar=__CreateRawText(${json(rawText)});")
        append("__AppendElement($variableName,$rawTextVar);")
      }

      node.children.forEachIndexed { index, child ->
        val childVar = "${variableName}_child_$index"
        append(renderCreateNode(child, childVar))
        append("__AppendElement($variableName,$childVar);")
      }
    }
  }

  private fun renderStyle(styles: Map<String, String>): String {
    if (styles.isEmpty()) {
      return ""
    }
    return styles.entries.joinToString(separator = ";", postfix = ";") { (key, value) ->
      val resolvedKey = when (key) {
        "display" -> if (value == "flex") "display" else key
        "flex-direction" -> "linear-direction"
        else -> key
      }
      val resolvedValue = when {
        key == "display" && value == "flex" -> "linear"
        else -> value
      }
      "$resolvedKey:$resolvedValue"
    }
  }

  private fun json(value: String): String {
    return "\"" + value
      .replace("\\", "\\\\")
      .replace("\"", "\\\"")
      .replace("\n", "\\n") + "\""
  }
}
