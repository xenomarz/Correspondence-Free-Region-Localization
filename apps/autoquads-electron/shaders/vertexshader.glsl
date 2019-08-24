attribute float size;
attribute float selected;
varying float vSelected;
void main()
{
  vSelected = selected;
  vec4 mvPosition = modelViewMatrix * vec4(position, 1.0);
  gl_PointSize = size;
  gl_Position = projectionMatrix * mvPosition;
}