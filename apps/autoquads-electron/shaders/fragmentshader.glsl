uniform vec3 color;
uniform sampler2D textureSelected;
uniform sampler2D textureUnselected;
varying float vSelected;
void main() {
  gl_FragColor = vec4(color, 1.0);
  float selected = vSelected;
  if(selected > 0.0)
  {
    gl_FragColor = gl_FragColor * texture2D(textureSelected, gl_PointCoord);
  }
  else
  {
    gl_FragColor = gl_FragColor * texture2D(textureUnselected, gl_PointCoord);
  }
  if ( gl_FragColor.a < ALPHATEST ) discard;
}