import cm

# Hacky interface to avoid Swig linking issues and use ui namespace in python
hasFocus = cm.ui_hasFocus
begin = cm.ui_begin
end = cm.ui_end
modified = cm.ui_modified
dragger = cm.ui_dragger
highlightDragger = cm.ui_highlightDragger
line = cm.ui_line
handle = cm.ui_handle
lengthHandle = cm.ui_lengthHandle
affineSimple = cm.ui_affineSimple
selector = cm.ui_selector
dragDelta = cm.ui_dragDelta
toolbar = cm.ui_toolbar
demo = cm.ui_demo
text = cm.ui_text
textColor = cm.ui_textColor
modifierShift = cm.ui_modifierShift
modifierAlt = cm.ui_modifierAlt