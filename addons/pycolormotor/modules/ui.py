import cm

# Hacky interface to avoid Swig linking issues and use ui namespace in python
hasFocus = cm.ui_hasFocus
begin = cm.ui_begin
end = cm.ui_end
modified = cm.ui_modified
dragger = cm.ui_dragger
highlightDragger = cm.ui_highlightDragger
#line = cm.ui_line
handle = cm.ui_handle
lengthHandle = cm.ui_lengthHandle
affineSimple = cm.ui_affineSimple
selector = cm.ui_selector
dragDelta = cm.ui_dragDelta
toolbar = cm.ui_toolbar
demo = cm.ui_demo
#text = cm.ui_text
#textColor = cm.ui_textColor
modifierShift = cm.ui_modifierShift
modifierAlt = cm.ui_modifierAlt

beginChild = cm.ui_beginChild
endChild = cm.ui_endChild
sliderFloat = cm.ui_sliderFloat
button = cm.ui_button
checkbox = cm.ui_checkbox

def line(a, b, clr=[-1,-1,-1,1]):
    if len(clr)==3:
        clr = cm.ImColor(clr[0], clr[1], clr[2])
    else:
        clr = cm.ImColor(clr[0], clr[1], clr[2], clr[3])
    cm.ui_line(a, b, clr)

def text(pos, txt, clr=[-1,-1,-1,1]):
    if len(clr)==3:
        clr = cm.ImColor(clr[0], clr[1], clr[2])
    else:
        clr = cm.ImColor(clr[0], clr[1], clr[2], clr[3])
    cm.ui_text(pos, txt, clr)
    
