#define CELLS vec2(64.0,30.0)
#define FALLERS 14.0
#define FALLERHEIGHT 12.0

vec2 rand(vec2 uv) {
    return floor(abs(mod(cos(
        uv * 652.6345 + uv.yx * 534.375 +
        iTime * 0.0000005 * dot(uv, vec2(0.364, 0.934))),
     0.001)) * 16000.0);
}

float fallerSpeed(float col, float faller) {
    return mod(cos(col * 363.435  + faller * 234.323), 0.1) * 1.0 + 0.3;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    
    vec2 pix = mod(uv, 1.0/CELLS);
    vec2 cell = (uv - pix) * CELLS;
    pix *= CELLS * vec2(0.8, 1.0) + vec2(0.1, 0.0);
   
    float c = texture(iChannel0, (rand(cell) + pix) / 16.0).x;
    
    float b = 0.0;
    for (float i = 0.0; i < FALLERS; ++i) {
        float f = 3.0 - cell.y * 0.05 -
            mod((iTime + i * 3534.34) * fallerSpeed(cell.x, i), FALLERHEIGHT);
        if (f > 0.0 && f < 1.0)
            b += f;
    }
    
    fragColor = vec4(0.0, c * b, 0.0, 1.0);
}