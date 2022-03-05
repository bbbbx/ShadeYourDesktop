/*
 * Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 * Contact: seplanely@gmail.com
 * Adapted from https://thebookofshaders.com/12
 */
 
// cell number on height
float cellNum = 6.;
// whether cell center can move
bool cellMove = true;
// whether cell can growth
bool cellGrowth = true;
// max grow size 1/min_k
float min_k = .6;
// min grow size 1/max_k
float max_k = 1.2;
// time cost for growing
float cellCycle = 10.;

float TWO_PI = 6.2831;
float random( vec2 p ) {
    return fract(sin((dot(p,vec2(233.1,123.7)) + dot(p,vec2(12.5,753.3)))*45345.5434));
}

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.1233);
}

vec3 random3( vec2 p ) {
    return fract(sin(vec3(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3)),dot(p,vec2(231.1,343.2))))*23234.2431);
}

float getK(vec2 gradlePos, float time) {
    float init_phase = random(gradlePos);
    float a = (max_k - min_k) / 2.;
    float c = (min_k + max_k) / 2.;
    float k = a*sin(time*TWO_PI/cellCycle + TWO_PI * init_phase) + c;
    return k;
}

vec2 getCellCenter(vec2 gradlePos, float time) {
    vec2 point = random2(gradlePos);
    if(cellMove) {
        float T = clamp(6.*length(point), 4., 8.);
        point = 0.5 + 0.5*sin(time*TWO_PI/T + TWO_PI*point);
    }
    return point;
}

vec3 getColor(vec2 gradlePos, float time) {
    vec3 color = random3(gradlePos);
    color = 0.5 + 0.5*sin(color);
    return color;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 st = fragCoord.xy/iResolution.xy;
    st.x *= iResolution.x/iResolution.y;
    vec3 color = vec3(0.);

    // Scale
    st *= cellNum;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.;
    vec2 m_neighbor = vec2(0.);
    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            vec2 point = getCellCenter(i_st + neighbor, iTime);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;
            
            float dist = 0.;
            if(cellGrowth) {
                float k = getK(i_st + neighbor, iTime);
                dist = length(diff) * k / max_k;
            } else
                dist = length(diff);

            // Keep the closer distance
            if(dist < m_dist) {
                m_neighbor = neighbor;
                m_dist = min(m_dist, dist);
            }
        }
    }
    color = getColor(i_st + m_neighbor, iTime);

    // Draw the min distance (distance field)
    //color *= m_dist;

    // Draw cell center
    color += 1.-step(.02, m_dist);

    // Draw grid
    //color.rgb += step(.99, f_st.x) + step(.99, f_st.y);

    // Show isolines
    //color -= step(.7,abs(sin(27.0*m_dist)))*.5;

    fragColor = vec4(color,1.0);
}