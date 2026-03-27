'use strict';
var e = e || '__Card__';
(() => {
  let __composePage;
  let __composePageId;
  function __composeBuildPage() {
    let root_0 = __CreateView(__composePageId);
    __SetInlineStyles(
        root_0,
        'display:linear;linear-direction:column;background-color:#F5F7FA;');
    let root_0_child_0 = __CreateView(__composePageId);
    __SetInlineStyles(
        root_0_child_0,
        'display:linear;linear-direction:row;background-color:#FFD54F;');
    let root_0_child_0_child_0 = __CreateText(__composePageId);
    let root_0_child_0_child_0_text_0 = __CreateRawText('Row Left');
    __AppendElement(root_0_child_0_child_0, root_0_child_0_child_0_text_0);
    __AppendElement(root_0_child_0, root_0_child_0_child_0);
    let root_0_child_0_child_1 = __CreateText(__composePageId);
    let root_0_child_0_child_1_text_0 = __CreateRawText('Row Right');
    __AppendElement(root_0_child_0_child_1, root_0_child_0_child_1_text_0);
    __AppendElement(root_0_child_0, root_0_child_0_child_1);
    __AppendElement(root_0, root_0_child_0);
    let root_0_child_1 = __CreateView(__composePageId);
    __SetInlineStyles(
        root_0_child_1,
        'display:linear;linear-direction:column;background-color:#80DEEA;');
    let root_0_child_1_child_0 = __CreateText(__composePageId);
    let root_0_child_1_child_0_text_0 = __CreateRawText('Column Top');
    __AppendElement(root_0_child_1_child_0, root_0_child_1_child_0_text_0);
    __AppendElement(root_0_child_1, root_0_child_1_child_0);
    let root_0_child_1_child_1 = __CreateText(__composePageId);
    let root_0_child_1_child_1_text_0 = __CreateRawText('Column Bottom');
    __AppendElement(root_0_child_1_child_1, root_0_child_1_child_1_text_0);
    __AppendElement(root_0_child_1, root_0_child_1_child_1);
    __AppendElement(root_0, root_0_child_1);
    __AppendElement(__composePage, root_0);
  }
  function renderPage(data) {
    data = data || {};
    __composePage = __CreatePage('0', 0);
    __composePageId = __GetElementUniqueID(__composePage);
    __composeBuildPage();
    __FlushElementTree(__composePage);
  }
  function updatePage(data, options) {
    renderPage(data);
    if (options) {
      __FlushElementTree(__composePage, options);
    }
  }
  function updateGlobalProps(data, options) {
    if (options) {
      __FlushElementTree(__composePage, options);
    } else if (__composePage) {
      __FlushElementTree(__composePage);
    }
  }
  Object.assign(globalThis, {
    renderPage: renderPage,
    updatePage: updatePage,
    updateGlobalProps: updateGlobalProps,
    getPageData: function() {
      return null;
    },
    removeComponents: function() {}
  });
})();