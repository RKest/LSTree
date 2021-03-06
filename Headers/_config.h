#ifndef ARR_SIZE
#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

//Shape of the bark
#ifndef WALL_NO
#define WALL_NO 6
#endif

//DISTIRBUTION
#ifndef TREE_NO
#define TREE_NO 5 //The same should be defined in the Tree2 Shader and ShadowMap Shader
#endif

#ifndef TREE_AREA_RADIOUS
#define TREE_AREA_RADIOUS 5.0f
#endif

#ifndef TREE_MIN_PROXIMA
#define TREE_MIN_PROXIMA 0.3f
#endif

//DISPLAY
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 1920
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 1080
#endif

//SHADOW_MAP
#ifndef SHADOW_WIDTH 
#define SHADOW_WIDTH 2048
#endif

#ifndef SHADOW_HEIGHT  
#define SHADOW_HEIGHT 2048
#endif

#ifndef SHADOW_NEAR_PLANE  
#define SHADOW_NEAR_PLANE 0.1f
#endif

#ifndef SHADOW_FAR_PLANE  
#define SHADOW_FAR_PLANE 35.0f
#endif

//MESH_MAKER
#define SEGMNET_LENGTH 0.02f
#define SEGMENT_GURTH_INTERCEPT 0.15f
#define SEGMENT_GURTH_DECAY_RATE 0.99f
#define SEGMENT_GURTH_CONSTANT 0.005f
#define SEGMENT_ROTATION 40.0f

//MESH:Joints
#define MAX_JOINT_DEPTH 3
#define MAX_JOINT_AMOUNT 13 //The same should be defined in the Tree2 Shader and ShadowMap Shader

//WIND
#define WIND_GUST_ODDS 0.01f
#define WIND_GUST_DECAY_RATE 0.7f
#define WIND_GUST_STRENGTH 0.01f
#define WIND_DECAY_RATE 0.995f
#define WIND_DIRECTION_VECTOR glm::vec3(0.0f, 0.0f, 1.0f)
#define WIND_AFFECTION_ANGLE 40.0f
#define WIND_MAX_WIND_COUNTER 1.0f

typedef unsigned long long u64; 
typedef unsigned int ui;
typedef float ft;