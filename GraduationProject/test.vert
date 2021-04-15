#version 330 core
layout(location = 0)in float inValue;
out float outValue;
void main()
{
   outValue = sqrt(inValue);
}