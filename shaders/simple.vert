#version 120

invariant gl_Position;
//attribute vec2 position;
//attribute vec2 offset;

void main(void) {
  gl_Position = ftransform();
  gl_FrontColor = gl_Color;
}
