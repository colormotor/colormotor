rm cm_wrap.h
rm cm_wrap.cpp
rm cm.py

echo "swig:wrapping colormotor..."

swig -w322,362 -python -c++ -extranative  -I./../../src cm.i

echo "void initializeSwig_cm() { SWIG_init(); }" >> cm_wrap.cxx
echo "void initializeSwig_cm();" >> cm_wrap.h

echo "copying files"
# cp app.py ../../modules/app.py
mkdir modules
cp cm.py modules/cm.py

