//-----------------------------------------------------------------------------
// Quake GL to DirectX wrapper
//-----------------------------------------------------------------------------

#ifndef _OPENGL32_H_
#define _OPENGL32_H_

#ifdef DECLSPEC_IMPORT
#undef DECLSPEC_IMPORT
#endif
#define DECLSPEC_IMPORT __declspec(dllexport)

#define CINTERFACE
#include <../dx6sdk/include/ddraw.h>
#include <../dx6sdk/include/d3d.h>

#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT __declspec(dllimport)

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

/*************************************************************/

/* Version */
#define GL_VERSION_1_1                    1

/* AccumOp */
#define GL_ACCUM                          0x0100
#define GL_LOAD                           0x0101
#define GL_RETURN                         0x0102
#define GL_MULT                           0x0103
#define GL_ADD                            0x0104

/* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

/* AttribMask */
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009

/* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305

/* BlendingFactorSrc */
/*      GL_ZERO */
/*      GL_ONE */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
/*      GL_SRC_ALPHA */
/*      GL_ONE_MINUS_SRC_ALPHA */
/*      GL_DST_ALPHA */
/*      GL_ONE_MINUS_DST_ALPHA */

/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0

/* ClearBufferMask */
/*      GL_COLOR_BUFFER_BIT */
/*      GL_ACCUM_BUFFER_BIT */
/*      GL_STENCIL_BUFFER_BIT */
/*      GL_DEPTH_BUFFER_BIT */

/* ClientArrayType */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */

/* ClipPlaneName */
#define GL_CLIP_PLANE0                    0x3000
#define GL_CLIP_PLANE1                    0x3001
#define GL_CLIP_PLANE2                    0x3002
#define GL_CLIP_PLANE3                    0x3003
#define GL_CLIP_PLANE4                    0x3004
#define GL_CLIP_PLANE5                    0x3005

/* ColorMaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */
/*      GL_EMISSION */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* CullFaceMode */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_2_BYTES                        0x1407
#define GL_3_BYTES                        0x1408
#define GL_4_BYTES                        0x1409
#define GL_DOUBLE                         0x140A

/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* DrawBufferMode */
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_AUX0                           0x0409
#define GL_AUX1                           0x040A
#define GL_AUX2                           0x040B
#define GL_AUX3                           0x040C

/* Enable */
/*      GL_FOG */
/*      GL_LIGHTING */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_LINE_STIPPLE */
/*      GL_POLYGON_STIPPLE */
/*      GL_CULL_FACE */
/*      GL_ALPHA_TEST */
/*      GL_BLEND */
/*      GL_INDEX_LOGIC_OP */
/*      GL_COLOR_LOGIC_OP */
/*      GL_DITHER */
/*      GL_STENCIL_TEST */
/*      GL_DEPTH_TEST */
/*      GL_CLIP_PLANE0 */
/*      GL_CLIP_PLANE1 */
/*      GL_CLIP_PLANE2 */
/*      GL_CLIP_PLANE3 */
/*      GL_CLIP_PLANE4 */
/*      GL_CLIP_PLANE5 */
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
/*      GL_TEXTURE_GEN_S */
/*      GL_TEXTURE_GEN_T */
/*      GL_TEXTURE_GEN_R */
/*      GL_TEXTURE_GEN_Q */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_POINT_SMOOTH */
/*      GL_LINE_SMOOTH */
/*      GL_POLYGON_SMOOTH */
/*      GL_SCISSOR_TEST */
/*      GL_COLOR_MATERIAL */
/*      GL_NORMALIZE */
/*      GL_AUTO_NORMAL */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_POLYGON_OFFSET_POINT */
/*      GL_POLYGON_OFFSET_LINE */
/*      GL_POLYGON_OFFSET_FILL */

/* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

/* FeedBackMode */
#define GL_2D                             0x0600
#define GL_3D                             0x0601
#define GL_3D_COLOR                       0x0602
#define GL_3D_COLOR_TEXTURE               0x0603
#define GL_4D_COLOR_TEXTURE               0x0604

/* FeedBackToken */
#define GL_PASS_THROUGH_TOKEN             0x0700
#define GL_POINT_TOKEN                    0x0701
#define GL_LINE_TOKEN                     0x0702
#define GL_POLYGON_TOKEN                  0x0703
#define GL_BITMAP_TOKEN                   0x0704
#define GL_DRAW_PIXEL_TOKEN               0x0705
#define GL_COPY_PIXEL_TOKEN               0x0706
#define GL_LINE_RESET_TOKEN               0x0707

/* FogMode */
/*      GL_LINEAR */
#define GL_EXP                            0x0800
#define GL_EXP2                           0x0801


/* FogParameter */
/*      GL_FOG_COLOR */
/*      GL_FOG_DENSITY */
/*      GL_FOG_END */
/*      GL_FOG_INDEX */
/*      GL_FOG_MODE */
/*      GL_FOG_START */

/* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901

/* GetMapTarget */
#define GL_COEFF                          0x0A00
#define GL_ORDER                          0x0A01
#define GL_DOMAIN                         0x0A02

/* GetPixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* GetPointerTarget */
/*      GL_VERTEX_ARRAY_POINTER */
/*      GL_NORMAL_ARRAY_POINTER */
/*      GL_COLOR_ARRAY_POINTER */
/*      GL_INDEX_ARRAY_POINTER */
/*      GL_TEXTURE_COORD_ARRAY_POINTER */
/*      GL_EDGE_FLAG_ARRAY_POINTER */

/* GetTarget */
#define GL_CURRENT_COLOR                  0x0B00
#define GL_CURRENT_INDEX                  0x0B01
#define GL_CURRENT_NORMAL                 0x0B02
#define GL_CURRENT_TEXTURE_COORDS         0x0B03
#define GL_CURRENT_RASTER_COLOR           0x0B04
#define GL_CURRENT_RASTER_INDEX           0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS  0x0B06
#define GL_CURRENT_RASTER_POSITION        0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID  0x0B08
#define GL_CURRENT_RASTER_DISTANCE        0x0B09
#define GL_POINT_SMOOTH                   0x0B10
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_LINE_STIPPLE                   0x0B24
#define GL_LINE_STIPPLE_PATTERN           0x0B25
#define GL_LINE_STIPPLE_REPEAT            0x0B26
#define GL_LIST_MODE                      0x0B30
#define GL_MAX_LIST_NESTING               0x0B31
#define GL_LIST_BASE                      0x0B32
#define GL_LIST_INDEX                     0x0B33
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_POLYGON_STIPPLE                0x0B42
#define GL_EDGE_FLAG                      0x0B43
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_LIGHTING                       0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER       0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE           0x0B52
#define GL_LIGHT_MODEL_AMBIENT            0x0B53
#define GL_SHADE_MODEL                    0x0B54
#define GL_COLOR_MATERIAL_FACE            0x0B55
#define GL_COLOR_MATERIAL_PARAMETER       0x0B56
#define GL_COLOR_MATERIAL                 0x0B57
#define GL_FOG                            0x0B60
#define GL_FOG_INDEX                      0x0B61
#define GL_FOG_DENSITY                    0x0B62
#define GL_FOG_START                      0x0B63
#define GL_FOG_END                        0x0B64
#define GL_FOG_MODE                       0x0B65
#define GL_FOG_COLOR                      0x0B66
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_ACCUM_CLEAR_VALUE              0x0B80
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_MATRIX_MODE                    0x0BA0
#define GL_NORMALIZE                      0x0BA1
#define GL_VIEWPORT                       0x0BA2
#define GL_MODELVIEW_STACK_DEPTH          0x0BA3
#define GL_PROJECTION_STACK_DEPTH         0x0BA4
#define GL_TEXTURE_STACK_DEPTH            0x0BA5
#define GL_MODELVIEW_MATRIX               0x0BA6
#define GL_PROJECTION_MATRIX              0x0BA7
#define GL_TEXTURE_MATRIX                 0x0BA8
#define GL_ATTRIB_STACK_DEPTH             0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH      0x0BB1
#define GL_ALPHA_TEST                     0x0BC0
#define GL_ALPHA_TEST_FUNC                0x0BC1
#define GL_ALPHA_TEST_REF                 0x0BC2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_INDEX_LOGIC_OP                 0x0BF1
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_AUX_BUFFERS                    0x0C00
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_INDEX_CLEAR_VALUE              0x0C20
#define GL_INDEX_WRITEMASK                0x0C21
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_INDEX_MODE                     0x0C30
#define GL_RGBA_MODE                      0x0C31
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_RENDER_MODE                    0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define GL_POINT_SMOOTH_HINT              0x0C51
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_FOG_HINT                       0x0C54
#define GL_TEXTURE_GEN_S                  0x0C60
#define GL_TEXTURE_GEN_T                  0x0C61
#define GL_TEXTURE_GEN_R                  0x0C62
#define GL_TEXTURE_GEN_Q                  0x0C63
#define GL_PIXEL_MAP_I_TO_I               0x0C70
#define GL_PIXEL_MAP_S_TO_S               0x0C71
#define GL_PIXEL_MAP_I_TO_R               0x0C72
#define GL_PIXEL_MAP_I_TO_G               0x0C73
#define GL_PIXEL_MAP_I_TO_B               0x0C74
#define GL_PIXEL_MAP_I_TO_A               0x0C75
#define GL_PIXEL_MAP_R_TO_R               0x0C76
#define GL_PIXEL_MAP_G_TO_G               0x0C77
#define GL_PIXEL_MAP_B_TO_B               0x0C78
#define GL_PIXEL_MAP_A_TO_A               0x0C79
#define GL_PIXEL_MAP_I_TO_I_SIZE          0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE          0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE          0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE          0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE          0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE          0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE          0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE          0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE          0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE          0x0CB9
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAP_COLOR                      0x0D10
#define GL_MAP_STENCIL                    0x0D11
#define GL_INDEX_SHIFT                    0x0D12
#define GL_INDEX_OFFSET                   0x0D13
#define GL_RED_SCALE                      0x0D14
#define GL_RED_BIAS                       0x0D15
#define GL_ZOOM_X                         0x0D16
#define GL_ZOOM_Y                         0x0D17
#define GL_GREEN_SCALE                    0x0D18
#define GL_GREEN_BIAS                     0x0D19
#define GL_BLUE_SCALE                     0x0D1A
#define GL_BLUE_BIAS                      0x0D1B
#define GL_ALPHA_SCALE                    0x0D1C
#define GL_ALPHA_BIAS                     0x0D1D
#define GL_DEPTH_SCALE                    0x0D1E
#define GL_DEPTH_BIAS                     0x0D1F
#define GL_MAX_EVAL_ORDER                 0x0D30
#define GL_MAX_LIGHTS                     0x0D31
#define GL_MAX_CLIP_PLANES                0x0D32
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_PIXEL_MAP_TABLE            0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH         0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define GL_MAX_NAME_STACK_DEPTH           0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH  0x0D3B
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_INDEX_BITS                     0x0D51
#define GL_RED_BITS                       0x0D52
#define GL_GREEN_BITS                     0x0D53
#define GL_BLUE_BITS                      0x0D54
#define GL_ALPHA_BITS                     0x0D55
#define GL_DEPTH_BITS                     0x0D56
#define GL_STENCIL_BITS                   0x0D57
#define GL_ACCUM_RED_BITS                 0x0D58
#define GL_ACCUM_GREEN_BITS               0x0D59
#define GL_ACCUM_BLUE_BITS                0x0D5A
#define GL_ACCUM_ALPHA_BITS               0x0D5B
#define GL_NAME_STACK_DEPTH               0x0D70
#define GL_AUTO_NORMAL                    0x0D80
#define GL_MAP1_COLOR_4                   0x0D90
#define GL_MAP1_INDEX                     0x0D91
#define GL_MAP1_NORMAL                    0x0D92
#define GL_MAP1_TEXTURE_COORD_1           0x0D93
#define GL_MAP1_TEXTURE_COORD_2           0x0D94
#define GL_MAP1_TEXTURE_COORD_3           0x0D95
#define GL_MAP1_TEXTURE_COORD_4           0x0D96
#define GL_MAP1_VERTEX_3                  0x0D97
#define GL_MAP1_VERTEX_4                  0x0D98
#define GL_MAP2_COLOR_4                   0x0DB0
#define GL_MAP2_INDEX                     0x0DB1
#define GL_MAP2_NORMAL                    0x0DB2
#define GL_MAP2_TEXTURE_COORD_1           0x0DB3
#define GL_MAP2_TEXTURE_COORD_2           0x0DB4
#define GL_MAP2_TEXTURE_COORD_3           0x0DB5
#define GL_MAP2_TEXTURE_COORD_4           0x0DB6
#define GL_MAP2_VERTEX_3                  0x0DB7
#define GL_MAP2_VERTEX_4                  0x0DB8
#define GL_MAP1_GRID_DOMAIN               0x0DD0
#define GL_MAP1_GRID_SEGMENTS             0x0DD1
#define GL_MAP2_GRID_DOMAIN               0x0DD2
#define GL_MAP2_GRID_SEGMENTS             0x0DD3
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_FEEDBACK_BUFFER_POINTER        0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE           0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE           0x0DF2
#define GL_SELECTION_BUFFER_POINTER       0x0DF3
#define GL_SELECTION_BUFFER_SIZE          0x0DF4
/*      GL_TEXTURE_BINDING_1D */
/*      GL_TEXTURE_BINDING_2D */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_VERTEX_ARRAY_SIZE */
/*      GL_VERTEX_ARRAY_TYPE */
/*      GL_VERTEX_ARRAY_STRIDE */
/*      GL_NORMAL_ARRAY_TYPE */
/*      GL_NORMAL_ARRAY_STRIDE */
/*      GL_COLOR_ARRAY_SIZE */
/*      GL_COLOR_ARRAY_TYPE */
/*      GL_COLOR_ARRAY_STRIDE */
/*      GL_INDEX_ARRAY_TYPE */
/*      GL_INDEX_ARRAY_STRIDE */
/*      GL_TEXTURE_COORD_ARRAY_SIZE */
/*      GL_TEXTURE_COORD_ARRAY_TYPE */
/*      GL_TEXTURE_COORD_ARRAY_STRIDE */
/*      GL_EDGE_FLAG_ARRAY_STRIDE */
/*      GL_POLYGON_OFFSET_FACTOR */
/*      GL_POLYGON_OFFSET_UNITS */

/* GetTextureParameter */
/*      GL_TEXTURE_MAG_FILTER */
/*      GL_TEXTURE_MIN_FILTER */
/*      GL_TEXTURE_WRAP_S */
/*      GL_TEXTURE_WRAP_T */
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_TEXTURE_BORDER                 0x1005
/*      GL_TEXTURE_RED_SIZE */
/*      GL_TEXTURE_GREEN_SIZE */
/*      GL_TEXTURE_BLUE_SIZE */
/*      GL_TEXTURE_ALPHA_SIZE */
/*      GL_TEXTURE_LUMINANCE_SIZE */
/*      GL_TEXTURE_INTENSITY_SIZE */
/*      GL_TEXTURE_PRIORITY */
/*      GL_TEXTURE_RESIDENT */

/* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102

/* HintTarget */
/*      GL_PERSPECTIVE_CORRECTION_HINT */
/*      GL_POINT_SMOOTH_HINT */
/*      GL_LINE_SMOOTH_HINT */
/*      GL_POLYGON_SMOOTH_HINT */
/*      GL_FOG_HINT */
/*      GL_PHONG_HINT */

/* IndexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* LightModelParameter */
/*      GL_LIGHT_MODEL_AMBIENT */
/*      GL_LIGHT_MODEL_LOCAL_VIEWER */
/*      GL_LIGHT_MODEL_TWO_SIDE */

/* LightName */
#define GL_LIGHT0                         0x4000
#define GL_LIGHT1                         0x4001
#define GL_LIGHT2                         0x4002
#define GL_LIGHT3                         0x4003
#define GL_LIGHT4                         0x4004
#define GL_LIGHT5                         0x4005
#define GL_LIGHT6                         0x4006
#define GL_LIGHT7                         0x4007

/* LightParameter */
#define GL_AMBIENT                        0x1200
#define GL_DIFFUSE                        0x1201
#define GL_SPECULAR                       0x1202
#define GL_POSITION                       0x1203
#define GL_SPOT_DIRECTION                 0x1204
#define GL_SPOT_EXPONENT                  0x1205
#define GL_SPOT_CUTOFF                    0x1206
#define GL_CONSTANT_ATTENUATION           0x1207
#define GL_LINEAR_ATTENUATION             0x1208
#define GL_QUADRATIC_ATTENUATION          0x1209

/* InterleavedArrays */
/*      GL_V2F */
/*      GL_V3F */
/*      GL_C4UB_V2F */
/*      GL_C4UB_V3F */
/*      GL_C3F_V3F */
/*      GL_N3F_V3F */
/*      GL_C4F_N3F_V3F */
/*      GL_T2F_V3F */
/*      GL_T4F_V4F */
/*      GL_T2F_C4UB_V3F */
/*      GL_T2F_C3F_V3F */
/*      GL_T2F_N3F_V3F */
/*      GL_T2F_C4F_N3F_V3F */
/*      GL_T4F_C4F_N3F_V4F */

/* ListMode */
#define GL_COMPILE                        0x1300
#define GL_COMPILE_AND_EXECUTE            0x1301

/* ListNameType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_2_BYTES */
/*      GL_3_BYTES */
/*      GL_4_BYTES */

/* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/* MapTarget */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */

/* MaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
#define GL_EMISSION                       0x1600
#define GL_SHININESS                      0x1601
#define GL_AMBIENT_AND_DIFFUSE            0x1602
#define GL_COLOR_INDEXES                  0x1603
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702

/* MeshMode1 */
/*      GL_POINT */
/*      GL_LINE */

/* MeshMode2 */
/*      GL_POINT */
/*      GL_LINE */
/*      GL_FILL */

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* PixelCopyType */
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802

/* PixelFormat */
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

/* PixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* PixelStore */
/*      GL_UNPACK_SWAP_BYTES */
/*      GL_UNPACK_LSB_FIRST */
/*      GL_UNPACK_ROW_LENGTH */
/*      GL_UNPACK_SKIP_ROWS */
/*      GL_UNPACK_SKIP_PIXELS */
/*      GL_UNPACK_ALIGNMENT */
/*      GL_PACK_SWAP_BYTES */
/*      GL_PACK_LSB_FIRST */
/*      GL_PACK_ROW_LENGTH */
/*      GL_PACK_SKIP_ROWS */
/*      GL_PACK_SKIP_PIXELS */
/*      GL_PACK_ALIGNMENT */

/* PixelTransfer */
/*      GL_MAP_COLOR */
/*      GL_MAP_STENCIL */
/*      GL_INDEX_SHIFT */
/*      GL_INDEX_OFFSET */
/*      GL_RED_SCALE */
/*      GL_RED_BIAS */
/*      GL_GREEN_SCALE */
/*      GL_GREEN_BIAS */
/*      GL_BLUE_SCALE */
/*      GL_BLUE_BIAS */
/*      GL_ALPHA_SCALE */
/*      GL_ALPHA_BIAS */
/*      GL_DEPTH_SCALE */
/*      GL_DEPTH_BIAS */

/* PixelType */
#define GL_BITMAP                         0x1A00
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */

/* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02

/* ReadBufferMode */
/*      GL_FRONT_LEFT */
/*      GL_FRONT_RIGHT */
/*      GL_BACK_LEFT */
/*      GL_BACK_RIGHT */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_LEFT */
/*      GL_RIGHT */
/*      GL_AUX0 */
/*      GL_AUX1 */
/*      GL_AUX2 */
/*      GL_AUX3 */

/* RenderingMode */
#define GL_RENDER                         0x1C00
#define GL_FEEDBACK                       0x1C01
#define GL_SELECT                         0x1C02

/* ShadingModel */
#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01


/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
/*      GL_INVERT */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TextureCoordName */
#define GL_S                              0x2000
#define GL_T                              0x2001
#define GL_R                              0x2002
#define GL_Q                              0x2003

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* TextureEnvMode */
#define GL_MODULATE                       0x2100
#define GL_DECAL                          0x2101
/*      GL_BLEND */
/*      GL_REPLACE */

/* TextureEnvParameter */
#define GL_TEXTURE_ENV_MODE               0x2200
#define GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define GL_TEXTURE_ENV                    0x2300

/* TextureGenMode */
#define GL_EYE_LINEAR                     0x2400
#define GL_OBJECT_LINEAR                  0x2401
#define GL_SPHERE_MAP                     0x2402

/* TextureGenParameter */
#define GL_TEXTURE_GEN_MODE               0x2500
#define GL_OBJECT_PLANE                   0x2501
#define GL_EYE_PLANE                      0x2502

/* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
/*      GL_TEXTURE_BORDER_COLOR */
/*      GL_TEXTURE_PRIORITY */

/* TextureTarget */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_PROXY_TEXTURE_1D */
/*      GL_PROXY_TEXTURE_2D */

/* TextureWrapMode */
#define GL_CLAMP                          0x2900
#define GL_REPEAT                         0x2901

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* ClientAttribMask */
#define GL_CLIENT_PIXEL_STORE_BIT         0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT        0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS         0xffffffff

/* polygon_offset */
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037

/* texture */
#define GL_ALPHA4                         0x803B
#define GL_ALPHA8                         0x803C
#define GL_ALPHA12                        0x803D
#define GL_ALPHA16                        0x803E
#define GL_LUMINANCE4                     0x803F
#define GL_LUMINANCE8                     0x8040
#define GL_LUMINANCE12                    0x8041
#define GL_LUMINANCE16                    0x8042
#define GL_LUMINANCE4_ALPHA4              0x8043
#define GL_LUMINANCE6_ALPHA2              0x8044
#define GL_LUMINANCE8_ALPHA8              0x8045
#define GL_LUMINANCE12_ALPHA4             0x8046
#define GL_LUMINANCE12_ALPHA12            0x8047
#define GL_LUMINANCE16_ALPHA16            0x8048
#define GL_INTENSITY                      0x8049
#define GL_INTENSITY4                     0x804A
#define GL_INTENSITY8                     0x804B
#define GL_INTENSITY12                    0x804C
#define GL_INTENSITY16                    0x804D
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_LUMINANCE_SIZE         0x8060
#define GL_TEXTURE_INTENSITY_SIZE         0x8061
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064

/* texture_object */
#define GL_TEXTURE_PRIORITY               0x8066
#define GL_TEXTURE_RESIDENT               0x8067
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069

/* vertex_array */
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_INDEX_ARRAY                    0x8077
#define GL_TEXTURE_COORD_ARRAY            0x8078
#define GL_EDGE_FLAG_ARRAY                0x8079
#define GL_VERTEX_ARRAY_SIZE              0x807A
#define GL_VERTEX_ARRAY_TYPE              0x807B
#define GL_VERTEX_ARRAY_STRIDE            0x807C
#define GL_NORMAL_ARRAY_TYPE              0x807E
#define GL_NORMAL_ARRAY_STRIDE            0x807F
#define GL_COLOR_ARRAY_SIZE               0x8081
#define GL_COLOR_ARRAY_TYPE               0x8082
#define GL_COLOR_ARRAY_STRIDE             0x8083
#define GL_INDEX_ARRAY_TYPE               0x8085
#define GL_INDEX_ARRAY_STRIDE             0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE       0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE       0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE     0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE         0x808C
#define GL_VERTEX_ARRAY_POINTER           0x808E
#define GL_NORMAL_ARRAY_POINTER           0x808F
#define GL_COLOR_ARRAY_POINTER            0x8090
#define GL_INDEX_ARRAY_POINTER            0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER    0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER        0x8093
#define GL_V2F                            0x2A20
#define GL_V3F                            0x2A21
#define GL_C4UB_V2F                       0x2A22
#define GL_C4UB_V3F                       0x2A23
#define GL_C3F_V3F                        0x2A24
#define GL_N3F_V3F                        0x2A25
#define GL_C4F_N3F_V3F                    0x2A26
#define GL_T2F_V3F                        0x2A27
#define GL_T4F_V4F                        0x2A28
#define GL_T2F_C4UB_V3F                   0x2A29
#define GL_T2F_C3F_V3F                    0x2A2A
#define GL_T2F_N3F_V3F                    0x2A2B
#define GL_T2F_C4F_N3F_V3F                0x2A2C
#define GL_T4F_C4F_N3F_V4F                0x2A2D

/* Extensions */
#define GL_EXT_vertex_array               1
#define GL_EXT_bgra                       1
#define GL_EXT_paletted_texture           1
#define GL_WIN_swap_hint                  1
#define GL_WIN_draw_range_elements        1
// #define GL_WIN_phong_shading              1
// #define GL_WIN_specular_fog               1

/* EXT_vertex_array */
#define GL_VERTEX_ARRAY_EXT               0x8074
#define GL_NORMAL_ARRAY_EXT               0x8075
#define GL_COLOR_ARRAY_EXT                0x8076
#define GL_INDEX_ARRAY_EXT                0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT        0x8078
#define GL_EDGE_FLAG_ARRAY_EXT            0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT          0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT          0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT        0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT         0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT          0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT        0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT         0x8080
#define GL_COLOR_ARRAY_SIZE_EXT           0x8081
#define GL_COLOR_ARRAY_TYPE_EXT           0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT         0x8083
#define GL_COLOR_ARRAY_COUNT_EXT          0x8084
#define GL_INDEX_ARRAY_TYPE_EXT           0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT         0x8086
#define GL_INDEX_ARRAY_COUNT_EXT          0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT      0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT       0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT       0x808F
#define GL_COLOR_ARRAY_POINTER_EXT        0x8090
#define GL_INDEX_ARRAY_POINTER_EXT        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT    0x8093
#define GL_DOUBLE_EXT                     GL_DOUBLE

/* EXT_bgra */
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1

/* EXT_paletted_texture */

/* These must match the GL_COLOR_TABLE_*_SGI enumerants */
#define GL_COLOR_TABLE_FORMAT_EXT         0x80D8
#define GL_COLOR_TABLE_WIDTH_EXT          0x80D9
#define GL_COLOR_TABLE_RED_SIZE_EXT       0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_EXT     0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_EXT      0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_EXT     0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_EXT 0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_EXT 0x80DF

#define GL_COLOR_INDEX1_EXT               0x80E2
#define GL_COLOR_INDEX2_EXT               0x80E3
#define GL_COLOR_INDEX4_EXT               0x80E4
#define GL_COLOR_INDEX8_EXT               0x80E5
#define GL_COLOR_INDEX12_EXT              0x80E6
#define GL_COLOR_INDEX16_EXT              0x80E7

/* WIN_draw_range_elements */
#define GL_MAX_ELEMENTS_VERTICES_WIN      0x80E8
#define GL_MAX_ELEMENTS_INDICES_WIN       0x80E9

/* WIN_phong_shading */
#define GL_PHONG_WIN                      0x80EA 
#define GL_PHONG_HINT_WIN                 0x80EB 

/* WIN_specular_fog */
#define GL_FOG_SPECULAR_TEXTURE_WIN       0x80EC

/* For compatibility with OpenGL v1.0 */
#define GL_LOGIC_OP GL_INDEX_LOGIC_OP
#define GL_TEXTURE_COMPONENTS GL_TEXTURE_INTERNAL_FORMAT

/*************************************************************/

extern "C" {
extern	void ( APIENTRY * qglAccum )( GLenum op, GLfloat value );
extern	void ( APIENTRY * qglAlphaFunc )( GLenum func, GLclampf ref );
extern	GLboolean ( APIENTRY * qglAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
extern	void ( APIENTRY * qglArrayElement )( GLint i );
extern	void ( APIENTRY * qglBegin )( GLenum mode );
extern	void ( APIENTRY * qglBindTexture )( GLenum target, GLuint texture );
extern	void ( APIENTRY * qglBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
extern	void ( APIENTRY * qglBlendFunc )( GLenum sfactor, GLenum dfactor );
extern	void ( APIENTRY * qglCallList )( GLuint list );
extern	void ( APIENTRY * qglCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
extern	void ( APIENTRY * qglClear )( GLbitfield mask );
extern	void ( APIENTRY * qglClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * qglClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
extern	void ( APIENTRY * qglClearDepth )( GLclampd depth );
extern	void ( APIENTRY * qglClearIndex )( GLfloat c );
extern	void ( APIENTRY * qglClearStencil )( GLint s );
extern	void ( APIENTRY * qglClipPlane )( GLenum plane, const GLdouble* equation );
extern	void ( APIENTRY * qglColor3b )( GLbyte red, GLbyte green, GLbyte blue );
extern	void ( APIENTRY * qglColor3bv )( const GLbyte* v );
extern	void ( APIENTRY * qglColor3d )( GLdouble red, GLdouble green, GLdouble blue );
extern	void ( APIENTRY * qglColor3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglColor3f )( GLfloat red, GLfloat green, GLfloat blue );
extern	void ( APIENTRY * qglColor3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglColor3i )( GLint red, GLint green, GLint blue );
extern	void ( APIENTRY * qglColor3iv )( const GLint* v );
extern	void ( APIENTRY * qglColor3s )( GLshort red, GLshort green, GLshort blue );
extern	void ( APIENTRY * qglColor3sv )( const GLshort* v );
extern	void ( APIENTRY * qglColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
extern	void ( APIENTRY * qglColor3ubv )( const GLubyte* v );
extern	void ( APIENTRY * qglColor3ui )( GLuint red, GLuint green, GLuint blue );
extern	void ( APIENTRY * qglColor3uiv )( const GLuint* v );
extern	void ( APIENTRY * qglColor3us )( GLushort red, GLushort green, GLushort blue );
extern	void ( APIENTRY * qglColor3usv )( const GLushort* v );
extern	void ( APIENTRY * qglColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
extern	void ( APIENTRY * qglColor4bv )( const GLbyte* v );
extern	void ( APIENTRY * qglColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
extern	void ( APIENTRY * qglColor4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * qglColor4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
extern	void ( APIENTRY * qglColor4iv )( const GLint* v );
extern	void ( APIENTRY * qglColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
extern	void ( APIENTRY * qglColor4sv )( const GLshort* v );
extern	void ( APIENTRY * qglColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
extern	void ( APIENTRY * qglColor4ubv )( const GLubyte* v );
extern	void ( APIENTRY * qglColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
extern	void ( APIENTRY * qglColor4uiv )( const GLuint* v );
extern	void ( APIENTRY * qglColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
extern	void ( APIENTRY * qglColor4usv )( const GLushort* v );
extern	void ( APIENTRY * qglColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
extern	void ( APIENTRY * qglColorMaterial )( GLenum face, GLenum mode );
extern	void ( APIENTRY * qglColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
extern	void ( APIENTRY * qglCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
extern	void ( APIENTRY * qglCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
extern	void ( APIENTRY * qglCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
extern	void ( APIENTRY * qglCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
extern	void ( APIENTRY * qglCullFace )( GLenum mode );
extern	void ( APIENTRY * qglDeleteLists )( GLuint list, GLsizei range );
extern	void ( APIENTRY * qglDeleteTextures )( GLsizei n, const GLuint* textures );
extern	void ( APIENTRY * qglDepthFunc )( GLenum func );
extern	void ( APIENTRY * qglDepthMask )( GLboolean flag );
extern	void ( APIENTRY * qglDepthRange )( GLclampd zNear, GLclampd zFar );
extern	void ( APIENTRY * qglDisable )( GLenum cap );
extern	void ( APIENTRY * qglDisableClientState )( GLenum array );
extern	void ( APIENTRY * qglDrawArrays )( GLenum mode, GLint first, GLsizei count );
extern	void ( APIENTRY * qglDrawBuffer )( GLenum mode );
extern	void ( APIENTRY * qglDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
extern	void ( APIENTRY * qglDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglEdgeFlag )( GLboolean flag );
extern	void ( APIENTRY * qglEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglEdgeFlagv )( const GLboolean* flag );
extern	void ( APIENTRY * qglEnable )( GLenum cap );
extern	void ( APIENTRY * qglEnableClientState )( GLenum array );
extern	void ( APIENTRY * qglEnd )( void );
extern	void ( APIENTRY * qglEndList )( void );
extern	void ( APIENTRY * qglEvalCoord1d )( GLdouble u );
extern	void ( APIENTRY * qglEvalCoord1dv )( const GLdouble* u );
extern	void ( APIENTRY * qglEvalCoord1f )( GLfloat u );
extern	void ( APIENTRY * qglEvalCoord1fv )( const GLfloat* u );
extern	void ( APIENTRY * qglEvalCoord2d )( GLdouble u, GLdouble v );
extern	void ( APIENTRY * qglEvalCoord2dv )( const GLdouble* u );
extern	void ( APIENTRY * qglEvalCoord2f )( GLfloat u, GLfloat v );
extern	void ( APIENTRY * qglEvalCoord2fv )( const GLfloat* u );
extern	void ( APIENTRY * qglEvalMesh1 )( GLenum mode, GLint i1, GLint i2 );
extern	void ( APIENTRY * qglEvalMesh2 )( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
extern	void ( APIENTRY * qglEvalPoint1 )( GLint i );
extern	void ( APIENTRY * qglEvalPoint2 )( GLint i, GLint j );
extern	void ( APIENTRY * qglFeedbackBuffer )( GLsizei size, GLenum type, GLfloat* buffer );
extern	void ( APIENTRY * qglFinish )( void );
extern	void ( APIENTRY * qglFlush )( void );
extern	void ( APIENTRY * qglFogf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglFogfv )( GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglFogi )( GLenum pname, GLint param );
extern	void ( APIENTRY * qglFogiv )( GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglFrontFace )( GLenum mode );
extern	void ( APIENTRY * qglFrustum )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern	GLuint ( APIENTRY * qglGenLists )( GLsizei range );
extern	void ( APIENTRY * qglGenTextures )( GLsizei n, GLuint* textures );
extern	void ( APIENTRY * qglGetBooleanv )( GLenum pname, GLboolean* params );
extern	void ( APIENTRY * qglGetClipPlane )( GLenum plane, GLdouble* equation );
extern	void ( APIENTRY * qglGetDoublev )( GLenum pname, GLdouble* params );
extern	GLenum ( APIENTRY * qglGetError )( void );
extern	void ( APIENTRY * qglGetFloatv )( GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetIntegerv )( GLenum pname, GLint* params );
extern	void ( APIENTRY * qglGetLightfv )( GLenum light, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetLightiv )( GLenum light, GLenum pname, GLint* params );
extern	void ( APIENTRY * qglGetMapdv )( GLenum target, GLenum query, GLdouble* v );
extern	void ( APIENTRY * qglGetMapfv )( GLenum target, GLenum query, GLfloat* v );
extern	void ( APIENTRY * qglGetMapiv )( GLenum target, GLenum query, GLint* v );
extern	void ( APIENTRY * qglGetMaterialfv )( GLenum face, GLenum pname, GLfloat* params);
extern	void ( APIENTRY * qglGetMaterialiv )( GLenum face, GLenum pname, GLint* params);
extern	void ( APIENTRY * qglGetPixelMapfv )( GLenum map, GLfloat* values );
extern	void ( APIENTRY * qglGetPixelMapuiv )( GLenum map, GLuint* values );
extern	void ( APIENTRY * qglGetPixelMapusv )( GLenum map, GLushort* values );
extern	void ( APIENTRY * qglGetPointerv )( GLenum pname, GLvoid** params );
extern	void ( APIENTRY * qglGetPolygonStipple )( GLubyte* mask );
extern	const GLubyte * ( APIENTRY * qglGetString )( GLenum name );
extern	void ( APIENTRY * qglGetTexEnvfv )( GLenum target, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetTexEnviv )( GLenum target, GLenum pname, GLint* params );
extern	void ( APIENTRY * qglGetTexGendv )( GLenum coord, GLenum pname, GLdouble* params );
extern	void ( APIENTRY * qglGetTexGenfv )( GLenum coord, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetTexGeniv )( GLenum coord, GLenum pname, GLint* params );
extern	void ( APIENTRY * qglGetTexImage )( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels );
extern	void ( APIENTRY * qglGetTexLevelParameterfv )( GLenum target, GLint level, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetTexLevelParameteriv )( GLenum target, GLint level, GLenum pname, GLint* params );
extern	void ( APIENTRY * qglGetTexParameterfv )( GLenum target, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * qglGetTexParameteriv )( GLenum target, GLenum pname, GLint* params );
extern	void ( APIENTRY * qglHint )( GLenum target, GLenum mode );
extern	void ( APIENTRY * qglIndexMask )( GLuint mask );
extern	void ( APIENTRY * qglIndexPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglIndexd )( GLdouble c );
extern	void ( APIENTRY * qglIndexdv )( const GLdouble* c );
extern	void ( APIENTRY * qglIndexf )( GLfloat c );
extern	void ( APIENTRY * qglIndexfv )( const GLfloat* c );
extern	void ( APIENTRY * qglIndexi )( GLint c );
extern	void ( APIENTRY * qglIndexiv )( const GLint* c );
extern	void ( APIENTRY * qglIndexs )( GLshort c );
extern	void ( APIENTRY * qglIndexsv )( const GLshort* c );
extern	void ( APIENTRY * qglIndexub )( GLubyte c );
extern	void ( APIENTRY * qglIndexubv )( const GLubyte* c );
extern	void ( APIENTRY * qglInitNames )( void );
extern	void ( APIENTRY * qglInterleavedArrays )( GLenum format, GLsizei stride, const GLvoid* pointer );
extern	GLboolean ( APIENTRY * qglIsEnabled )( GLenum cap );
extern	GLboolean ( APIENTRY * qglIsList )( GLuint list );
extern	GLboolean ( APIENTRY * qglIsTexture )( GLuint texture );
extern	void ( APIENTRY * qglLightModelf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglLightModelfv )( GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglLightModeli )( GLenum pname, GLint param );
extern	void ( APIENTRY * qglLightModeliv )( GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglLightf )( GLenum light, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglLightfv )( GLenum light, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglLighti )( GLenum light, GLenum pname, GLint param );
extern	void ( APIENTRY * qglLightiv )( GLenum light, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglLineStipple )( GLint factor, GLushort pattern );
extern	void ( APIENTRY * qglLineWidth )( GLfloat width );
extern	void ( APIENTRY * qglListBase )( GLuint base );
extern	void ( APIENTRY * qglLoadIdentity )( void );
extern	void ( APIENTRY * qglLoadMatrixd )( const GLdouble* m );
extern	void ( APIENTRY * qglLoadMatrixf )( const GLfloat* m );
extern	void ( APIENTRY * qglLoadName )( GLuint name );
extern	void ( APIENTRY * qglLogicOp )( GLenum opcode );
extern	void ( APIENTRY * qglMap1d )( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points );
extern	void ( APIENTRY * qglMap1f )( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points );
extern	void ( APIENTRY * qglMap2d )( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points );
extern	void ( APIENTRY * qglMap2f )( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points );
extern	void ( APIENTRY * qglMapGrid1d )( GLint un, GLdouble u1, GLdouble u2 );
extern	void ( APIENTRY * qglMapGrid1f )( GLint un, GLfloat u1, GLfloat u2 );
extern	void ( APIENTRY * qglMapGrid2d )( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
extern	void ( APIENTRY * qglMapGrid2f )( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
extern	void ( APIENTRY * qglMaterialf )( GLenum face, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglMaterialfv )( GLenum face, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglMateriali )( GLenum face, GLenum pname, GLint param );
extern	void ( APIENTRY * qglMaterialiv )( GLenum face, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglMatrixMode )( GLenum mode );
extern	void ( APIENTRY * qglMultMatrixd )( const GLdouble* m );
extern	void ( APIENTRY * qglMultMatrixf )( const GLfloat* m );
extern	void ( APIENTRY * qglNewList )( GLuint list, GLenum mode );
extern	void ( APIENTRY * qglNormal3b )( GLbyte nx, GLbyte ny, GLbyte nz );
extern	void ( APIENTRY * qglNormal3bv )( const GLbyte* v );
extern	void ( APIENTRY * qglNormal3d )( GLdouble nx, GLdouble ny, GLdouble nz );
extern	void ( APIENTRY * qglNormal3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglNormal3f )( GLfloat nx, GLfloat ny, GLfloat nz );
extern	void ( APIENTRY * qglNormal3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglNormal3i )( GLint nx, GLint ny, GLint nz );
extern	void ( APIENTRY * qglNormal3iv )( const GLint* v );
extern	void ( APIENTRY * qglNormal3s )( GLshort nx, GLshort ny, GLshort nz );
extern	void ( APIENTRY * qglNormal3sv )( const GLshort* v );
extern	void ( APIENTRY * qglNormalPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglOrtho )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern	void ( APIENTRY * qglPassThrough )( GLfloat token );
extern	void ( APIENTRY * qglPixelMapfv )( GLenum map, GLsizei mapsize, const GLfloat* values );
extern	void ( APIENTRY * qglPixelMapuiv )( GLenum map, GLsizei mapsize, const GLuint* values );
extern	void ( APIENTRY * qglPixelMapusv )( GLenum map, GLsizei mapsize, const GLushort* values );
extern	void ( APIENTRY * qglPixelStoref )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglPixelStorei )( GLenum pname, GLint param );
extern	void ( APIENTRY * qglPixelTransferf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglPixelTransferi )( GLenum pname, GLint param );
extern	void ( APIENTRY * qglPixelZoom )( GLfloat xfactor, GLfloat yfactor );
extern	void ( APIENTRY * qglPointSize )( GLfloat size );
extern	void ( APIENTRY * qglPolygonMode )( GLenum face, GLenum mode );
extern	void ( APIENTRY * qglPolygonOffset )( GLfloat factor, GLfloat units );
extern	void ( APIENTRY * qglPolygonStipple )( const GLubyte* mask );
extern	void ( APIENTRY * qglPopAttrib )( void );
extern	void ( APIENTRY * qglPopClientAttrib )( void );
extern	void ( APIENTRY * qglPopMatrix )( void );
extern	void ( APIENTRY * qglPopName )( void );
extern	void ( APIENTRY * qglPrioritizeTextures )( GLsizei n, const GLuint* textures, const GLclampf* priorities );
extern	void ( APIENTRY * qglPushAttrib )( GLbitfield mask );
extern	void ( APIENTRY * qglPushClientAttrib )( GLbitfield mask );
extern	void ( APIENTRY * qglPushMatrix )( void );
extern	void ( APIENTRY * qglPushName )( GLuint name );
extern	void ( APIENTRY * qglRasterPos2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * qglRasterPos2dv )( const GLdouble* v );
extern	void ( APIENTRY * qglRasterPos2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * qglRasterPos2fv )( const GLfloat* v );
extern	void ( APIENTRY * qglRasterPos2i )( GLint x, GLint y );
extern	void ( APIENTRY * qglRasterPos2iv )( const GLint* v );
extern	void ( APIENTRY * qglRasterPos2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * qglRasterPos2sv )( const GLshort* v );
extern	void ( APIENTRY * qglRasterPos3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglRasterPos3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglRasterPos3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglRasterPos3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglRasterPos3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * qglRasterPos3iv )( const GLint* v );
extern	void ( APIENTRY * qglRasterPos3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * qglRasterPos3sv )( const GLshort* v );
extern	void ( APIENTRY * qglRasterPos4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * qglRasterPos4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglRasterPos4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * qglRasterPos4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglRasterPos4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * qglRasterPos4iv )( const GLint* v );
extern	void ( APIENTRY * qglRasterPos4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * qglRasterPos4sv )( const GLshort* v );
extern	void ( APIENTRY * qglReadBuffer )( GLenum mode );
extern	void ( APIENTRY * qglReadPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels );
extern	void ( APIENTRY * qglRectd )( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
extern	void ( APIENTRY * qglRectdv )( const GLdouble* v1, const GLdouble* v2 );
extern	void ( APIENTRY * qglRectf )( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
extern	void ( APIENTRY * qglRectfv )( const GLfloat* v1, const GLfloat* v2 );
extern	void ( APIENTRY * qglRecti )( GLint x1, GLint y1, GLint x2, GLint y2 );
extern	void ( APIENTRY * qglRectiv )( const GLint* v1, const GLint* v2 );
extern	void ( APIENTRY * qglRects )( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
extern	void ( APIENTRY * qglRectsv )( const GLshort* v1, const GLshort* v2 );
extern	GLint ( APIENTRY * qglRenderMode )( GLenum mode );
extern	void ( APIENTRY * qglRotated )( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglRotatef )( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglScaled )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglScalef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglScissor )( GLint x, GLint y, GLsizei width, GLsizei height );
extern	void ( APIENTRY * qglSelectBuffer )( GLsizei size, GLuint* buffer );
extern	void ( APIENTRY * qglShadeModel )( GLenum mode );
extern	void ( APIENTRY * qglStencilFunc )( GLenum func, GLint ref, GLuint mask );
extern	void ( APIENTRY * qglStencilMask )( GLuint mask );
extern	void ( APIENTRY * qglStencilOp )( GLenum fail, GLenum zfail, GLenum zpass );
extern	void ( APIENTRY * qglTexCoord1d )( GLdouble s );
extern	void ( APIENTRY * qglTexCoord1dv )( const GLdouble* v );
extern	void ( APIENTRY * qglTexCoord1f )( GLfloat s );
extern	void ( APIENTRY * qglTexCoord1fv )( const GLfloat* v );
extern	void ( APIENTRY * qglTexCoord1i )( GLint s );
extern	void ( APIENTRY * qglTexCoord1iv )( const GLint* v );
extern	void ( APIENTRY * qglTexCoord1s )( GLshort s );
extern	void ( APIENTRY * qglTexCoord1sv )( const GLshort* v );
extern	void ( APIENTRY * qglTexCoord2d )( GLdouble s, GLdouble t );
extern	void ( APIENTRY * qglTexCoord2dv )( const GLdouble* v );
extern	void ( APIENTRY * qglTexCoord2f )( GLfloat s, GLfloat t );
extern	void ( APIENTRY * qglTexCoord2fv )( const GLfloat* v );
extern	void ( APIENTRY * qglTexCoord2i )( GLint s, GLint t );
extern	void ( APIENTRY * qglTexCoord2iv )( const GLint* v );
extern	void ( APIENTRY * qglTexCoord2s )( GLshort s, GLshort t );
extern	void ( APIENTRY * qglTexCoord2sv )( const GLshort* v );
extern	void ( APIENTRY * qglTexCoord3d )( GLdouble s, GLdouble t, GLdouble r );
extern	void ( APIENTRY * qglTexCoord3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglTexCoord3f )( GLfloat s, GLfloat t, GLfloat r );
extern	void ( APIENTRY * qglTexCoord3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglTexCoord3i )( GLint s, GLint t, GLint r );
extern	void ( APIENTRY * qglTexCoord3iv )( const GLint* v );
extern	void ( APIENTRY * qglTexCoord3s )( GLshort s, GLshort t, GLshort r );
extern	void ( APIENTRY * qglTexCoord3sv )( const GLshort* v );
extern	void ( APIENTRY * qglTexCoord4d )( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
extern	void ( APIENTRY * qglTexCoord4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglTexCoord4f )( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
extern	void ( APIENTRY * qglTexCoord4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglTexCoord4i )( GLint s, GLint t, GLint r, GLint q );
extern	void ( APIENTRY * qglTexCoord4iv )( const GLint* v );
extern	void ( APIENTRY * qglTexCoord4s )( GLshort s, GLshort t, GLshort r, GLshort q );
extern	void ( APIENTRY * qglTexCoord4sv )( const GLshort* v );
extern	void ( APIENTRY * qglTexCoordPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglTexEnvf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglTexEnvfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglTexEnvi )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * qglTexEnviv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglTexGend )( GLenum coord, GLenum pname, GLdouble param );
extern	void ( APIENTRY * qglTexGendv )( GLenum coord, GLenum pname, const GLdouble* params );
extern	void ( APIENTRY * qglTexGenf )( GLenum coord, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglTexGenfv )( GLenum coord, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglTexGeni )( GLenum coord, GLenum pname, GLint param );
extern	void ( APIENTRY * qglTexGeniv )( GLenum coord, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexParameterf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * qglTexParameterfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * qglTexParameteri )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * qglTexParameteriv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * qglTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * qglTranslated )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglTranslatef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglVertex2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * qglVertex2dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * qglVertex2fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex2i )( GLint x, GLint y );
extern	void ( APIENTRY * qglVertex2iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * qglVertex2sv )( const GLshort* v );
extern	void ( APIENTRY * qglVertex3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * qglVertex3dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * qglVertex3fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * qglVertex3iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * qglVertex3sv )( const GLshort* v );
extern	void ( APIENTRY * qglVertex4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * qglVertex4dv )( const GLdouble* v );
extern	void ( APIENTRY * qglVertex4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * qglVertex4fv )( const GLfloat* v );
extern	void ( APIENTRY * qglVertex4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * qglVertex4iv )( const GLint* v );
extern	void ( APIENTRY * qglVertex4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * qglVertex4sv )( const GLshort* v );
extern	void ( APIENTRY * qglVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * qglViewport )( GLint x, GLint y, GLsizei width, GLsizei height );

extern	BOOL ( APIENTRY * qwglCopyContext )( HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask );
extern	HGLRC ( APIENTRY * qwglCreateContext )( HDC hdc );
extern	HGLRC ( APIENTRY * qwglCreateLayerContext )( HDC hdc, int iLayerPlan );
extern	BOOL ( APIENTRY * qwglDeleteContext )( HGLRC hglrc );
extern	BOOL ( APIENTRY * qwglDescribeLayerPlane )( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd );
extern	HGLRC ( APIENTRY * qwglGetCurrentContext )( VOID );
extern	HDC  ( APIENTRY * qwglGetCurrentDC )( VOID );
extern	int  ( APIENTRY * qwglGetLayerPaletteEntries )( HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF* pcr );
extern	PROC ( APIENTRY * qwglGetProcAddress )( LPCSTR lpszProc );
extern	BOOL ( APIENTRY * qwglMakeCurrent )( HDC hdc, HGLRC hglrc );
extern	BOOL ( APIENTRY * qwglRealizeLayerPalette )( HDC hdc, int iLayerPlane, BOOL bRealize );
extern	int  ( APIENTRY * qwglSetLayerPaletteEntries )( HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF* pcr );
extern	BOOL ( APIENTRY * qwglShareLists )( HGLRC hglrc1, HGLRC hglrc2 );
extern	BOOL ( APIENTRY * qwglSwapLayerBuffers )( HDC hdc, UINT fuPlanes );
extern	BOOL ( APIENTRY * qwglUseFontBitmaps )( HDC hdc, DWORD first, DWORD count, DWORD listBase );
extern	BOOL ( APIENTRY * qwglUseFontOutlines )( HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf );
extern	int  ( APIENTRY * qwglChoosePixelFormat )( HDC hdc, CONST PIXELFORMATDESCRIPTOR* ppfd );
extern	int  ( APIENTRY * qwglDescribePixelFormat )( HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd );
extern	int  ( APIENTRY * qwglGetPixelFormat )( HDC hdc );
extern	BOOL ( APIENTRY * qwglSetPixelFormat )( HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR* ppfd );
extern	BOOL ( APIENTRY * qwglSwapBuffers )( HDC hdc );
extern	BOOL ( APIENTRY * qwglSwapIntervalEXT )( int interval );
extern	void ( APIENTRY * qglPointParameterfEXT )( GLenum param, GLfloat value );
extern	void ( APIENTRY * qglPointParameterfvEXT )( GLenum param, const GLfloat* value );
extern	void ( APIENTRY * qglColorTableEXT )( int, int, int, int, int, const void* );
extern	void ( APIENTRY * qglSelectTextureSGIS )( GLenum );
extern	void ( APIENTRY * qglMTexCoord2fSGIS )( GLenum, GLfloat, GLfloat );

extern	void ( APIENTRY * dllAccum )( GLenum op, GLfloat value );
extern	void ( APIENTRY * dllAlphaFunc )( GLenum func, GLclampf ref );
extern	GLboolean ( APIENTRY * dllAreTexturesResident )( GLsizei n, const GLuint* textures, GLboolean* residences );
extern	void ( APIENTRY * dllArrayElement )( GLint i );
extern	void ( APIENTRY * dllBegin )( GLenum mode );
extern	void ( APIENTRY * dllBindTexture )( GLenum target, GLuint texture );
extern	void ( APIENTRY * dllBitmap )( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap );
extern	void ( APIENTRY * dllBlendFunc )( GLenum sfactor, GLenum dfactor );
extern	void ( APIENTRY * dllCallList )( GLuint list );
extern	void ( APIENTRY * dllCallLists )( GLsizei n, GLenum type, const GLvoid* lists );
extern	void ( APIENTRY * dllClear )(GLbitfield mask);
extern	void ( APIENTRY * dllClearAccum )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * dllClearColor )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
extern	void ( APIENTRY * dllClearDepth )( GLclampd depth );
extern	void ( APIENTRY * dllClearIndex )( GLfloat c );
extern	void ( APIENTRY * dllClearStencil )( GLint s );
extern	void ( APIENTRY * dllClipPlane )( GLenum plane, const GLdouble* equation );
extern	void ( APIENTRY * dllColor3b )( GLbyte red, GLbyte green, GLbyte blue );
extern	void ( APIENTRY * dllColor3bv )( const GLbyte* v );
extern	void ( APIENTRY * dllColor3d )( GLdouble red, GLdouble green, GLdouble blue );
extern	void ( APIENTRY * dllColor3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllColor3f )( GLfloat red, GLfloat green, GLfloat blue );
extern	void ( APIENTRY * dllColor3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllColor3i )( GLint red, GLint green, GLint blue );
extern	void ( APIENTRY * dllColor3iv )( const GLint* v );
extern	void ( APIENTRY * dllColor3s )( GLshort red, GLshort green, GLshort blue );
extern	void ( APIENTRY * dllColor3sv )( const GLshort* v );
extern	void ( APIENTRY * dllColor3ub )( GLubyte red, GLubyte green, GLubyte blue );
extern	void ( APIENTRY * dllColor3ubv )( const GLubyte* v );
extern	void ( APIENTRY * dllColor3ui )( GLuint red, GLuint green, GLuint blue );
extern	void ( APIENTRY * dllColor3uiv )( const GLuint* v );
extern	void ( APIENTRY * dllColor3us )( GLushort red, GLushort green, GLushort blue );
extern	void ( APIENTRY * dllColor3usv )( const GLushort* v );
extern	void ( APIENTRY * dllColor4b )( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
extern	void ( APIENTRY * dllColor4bv )( const GLbyte* v );
extern	void ( APIENTRY * dllColor4d )( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
extern	void ( APIENTRY * dllColor4dv )( const GLdouble* v );
extern	void ( APIENTRY * dllColor4f )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern	void ( APIENTRY * dllColor4fv )( const GLfloat* v );
extern	void ( APIENTRY * dllColor4i )( GLint red, GLint green, GLint blue, GLint alpha );
extern	void ( APIENTRY * dllColor4iv )( const GLint* v );
extern	void ( APIENTRY * dllColor4s )( GLshort red, GLshort green, GLshort blue, GLshort alpha );
extern	void ( APIENTRY * dllColor4sv )( const GLshort* v );
extern	void ( APIENTRY * dllColor4ub )( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
extern	void ( APIENTRY * dllColor4ubv )( const GLubyte* v );
extern	void ( APIENTRY * dllColor4ui )( GLuint red, GLuint green, GLuint blue, GLuint alpha );
extern	void ( APIENTRY * dllColor4uiv )( const GLuint* v );
extern	void ( APIENTRY * dllColor4us )( GLushort red, GLushort green, GLushort blue, GLushort alpha );
extern	void ( APIENTRY * dllColor4usv )( const GLushort* v );
extern	void ( APIENTRY * dllColorMask )( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
extern	void ( APIENTRY * dllColorMaterial )( GLenum face, GLenum mode );
extern	void ( APIENTRY * dllColorPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllCopyPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
extern	void ( APIENTRY * dllCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
extern	void ( APIENTRY * dllCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
extern	void ( APIENTRY * dllCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
extern	void ( APIENTRY * dllCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
extern	void ( APIENTRY * dllCullFace )( GLenum mode );
extern	void ( APIENTRY * dllDeleteLists )( GLuint list, GLsizei range );
extern	void ( APIENTRY * dllDeleteTextures )( GLsizei n, const GLuint* textures );
extern	void ( APIENTRY * dllDepthFunc )( GLenum func );
extern	void ( APIENTRY * dllDepthMask )( GLboolean flag );
extern	void ( APIENTRY * dllDepthRange )( GLclampd zNear, GLclampd zFar );
extern	void ( APIENTRY * dllDisable )( GLenum cap );
extern	void ( APIENTRY * dllDisableClientState )( GLenum array );
extern	void ( APIENTRY * dllDrawArrays )( GLenum mode, GLint first, GLsizei count );
extern	void ( APIENTRY * dllDrawBuffer )( GLenum mode );
extern	void ( APIENTRY * dllDrawElements )( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices );
extern	void ( APIENTRY * dllDrawPixels )( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllEdgeFlag )( GLboolean flag );
extern	void ( APIENTRY * dllEdgeFlagPointer )( GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllEdgeFlagv )( const GLboolean* flag );
extern	void ( APIENTRY * dllEnable )( GLenum cap );
extern	void ( APIENTRY * dllEnableClientState )( GLenum array );
extern	void ( APIENTRY * dllEnd )( void );
extern	void ( APIENTRY * dllEndList )( void );
extern	void ( APIENTRY * dllEvalCoord1d )( GLdouble u );
extern	void ( APIENTRY * dllEvalCoord1dv )( const GLdouble* u );
extern	void ( APIENTRY * dllEvalCoord1f )( GLfloat u );
extern	void ( APIENTRY * dllEvalCoord1fv )( const GLfloat* u );
extern	void ( APIENTRY * dllEvalCoord2d )( GLdouble u, GLdouble v );
extern	void ( APIENTRY * dllEvalCoord2dv )( const GLdouble* u );
extern	void ( APIENTRY * dllEvalCoord2f )( GLfloat u, GLfloat v );
extern	void ( APIENTRY * dllEvalCoord2fv )( const GLfloat* u );
extern	void ( APIENTRY * dllEvalMesh1 )( GLenum mode, GLint i1, GLint i2 );
extern	void ( APIENTRY * dllEvalMesh2 )( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
extern	void ( APIENTRY * dllEvalPoint1 )( GLint i );
extern	void ( APIENTRY * dllEvalPoint2 )( GLint i, GLint j );
extern	void ( APIENTRY * dllFeedbackBuffer )( GLsizei size, GLenum type, GLfloat* buffer );
extern	void ( APIENTRY * dllFinish )( void );
extern	void ( APIENTRY * dllFlush )( void );
extern	void ( APIENTRY * dllFogf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllFogfv )( GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllFogi )( GLenum pname, GLint param );
extern	void ( APIENTRY * dllFogiv )( GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllFrontFace )( GLenum mode );
extern	void ( APIENTRY * dllFrustum )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern	GLuint ( APIENTRY * dllGenLists )( GLsizei range );
extern	void ( APIENTRY * dllGenTextures )( GLsizei n, GLuint* textures );
extern	void ( APIENTRY * dllGetBooleanv )( GLenum pname, GLboolean* params );
extern	void ( APIENTRY * dllGetClipPlane )( GLenum plane, GLdouble* equation );
extern	void ( APIENTRY * dllGetDoublev )( GLenum pname, GLdouble* params );
extern	GLenum ( APIENTRY * dllGetError )( void );
extern	void ( APIENTRY * dllGetFloatv )( GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetIntegerv )( GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetLightfv )( GLenum light, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetLightiv )( GLenum light, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetMapdv )( GLenum target, GLenum query, GLdouble* v );
extern	void ( APIENTRY * dllGetMapfv )( GLenum target, GLenum query, GLfloat* v );
extern	void ( APIENTRY * dllGetMapiv )( GLenum target, GLenum query, GLint* v );
extern	void ( APIENTRY * dllGetMaterialfv )( GLenum face, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetMaterialiv )( GLenum face, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetPixelMapfv )( GLenum map, GLfloat* values );
extern	void ( APIENTRY * dllGetPixelMapuiv )( GLenum map, GLuint* values );
extern	void ( APIENTRY * dllGetPixelMapusv )( GLenum map, GLushort* values );
extern	void ( APIENTRY * dllGetPointerv )( GLenum pname, GLvoid** params );
extern	void ( APIENTRY * dllGetPolygonStipple )( GLubyte* mask );
extern	const GLubyte * ( APIENTRY * dllGetString )( GLenum name );
extern	void ( APIENTRY * dllGetTexEnvfv )( GLenum target, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetTexEnviv )( GLenum target, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetTexGendv )( GLenum coord, GLenum pname, GLdouble* params );
extern	void ( APIENTRY * dllGetTexGenfv )( GLenum coord, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetTexGeniv )( GLenum coord, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetTexImage )( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels );
extern	void ( APIENTRY * dllGetTexLevelParameterfv )( GLenum target, GLint level, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetTexLevelParameteriv )( GLenum target, GLint level, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllGetTexParameterfv )( GLenum target, GLenum pname, GLfloat* params );
extern	void ( APIENTRY * dllGetTexParameteriv )( GLenum target, GLenum pname, GLint* params );
extern	void ( APIENTRY * dllHint )( GLenum target, GLenum mode );
extern	void ( APIENTRY * dllIndexMask )( GLuint mask );
extern	void ( APIENTRY * dllIndexPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllIndexd )( GLdouble c );
extern	void ( APIENTRY * dllIndexdv )( const GLdouble* c );
extern	void ( APIENTRY * dllIndexf )( GLfloat c );
extern	void ( APIENTRY * dllIndexfv )( const GLfloat* c );
extern	void ( APIENTRY * dllIndexi )( GLint c );
extern	void ( APIENTRY * dllIndexiv )( const GLint* c );
extern	void ( APIENTRY * dllIndexs )( GLshort c );
extern	void ( APIENTRY * dllIndexsv )( const GLshort* c );
extern	void ( APIENTRY * dllIndexub )( GLubyte c );
extern	void ( APIENTRY * dllIndexubv )( const GLubyte* c );
extern	void ( APIENTRY * dllInitNames )( void );
extern	void ( APIENTRY * dllInterleavedArrays )( GLenum format, GLsizei stride, const GLvoid* pointer );
extern	GLboolean ( APIENTRY * dllIsEnabled )( GLenum cap );
extern	GLboolean ( APIENTRY * dllIsList )( GLuint list );
extern	GLboolean ( APIENTRY * dllIsTexture )( GLuint texture );
extern	void ( APIENTRY * dllLightModelf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllLightModelfv )( GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllLightModeli )( GLenum pname, GLint param );
extern	void ( APIENTRY * dllLightModeliv )( GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllLightf )( GLenum light, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllLightfv )( GLenum light, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllLighti )( GLenum light, GLenum pname, GLint param );
extern	void ( APIENTRY * dllLightiv )( GLenum light, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllLineStipple )( GLint factor, GLushort pattern );
extern	void ( APIENTRY * dllLineWidth )( GLfloat width );
extern	void ( APIENTRY * dllListBase )( GLuint base );
extern	void ( APIENTRY * dllLoadIdentity )( void );
extern	void ( APIENTRY * dllLoadMatrixd )( const GLdouble* m );
extern	void ( APIENTRY * dllLoadMatrixf )( const GLfloat* m );
extern	void ( APIENTRY * dllLoadName )( GLuint name );
extern	void ( APIENTRY * dllLogicOp )( GLenum opcode );
extern	void ( APIENTRY * dllMap1d )( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points );
extern	void ( APIENTRY * dllMap1f )( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points );
extern	void ( APIENTRY * dllMap2d )( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points );
extern	void ( APIENTRY * dllMap2f )( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points );
extern	void ( APIENTRY * dllMapGrid1d )( GLint un, GLdouble u1, GLdouble u2 );
extern	void ( APIENTRY * dllMapGrid1f )( GLint un, GLfloat u1, GLfloat u2 );
extern	void ( APIENTRY * dllMapGrid2d )( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
extern	void ( APIENTRY * dllMapGrid2f )( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
extern	void ( APIENTRY * dllMaterialf )( GLenum face, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllMaterialfv )( GLenum face, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllMateriali )( GLenum face, GLenum pname, GLint param );
extern	void ( APIENTRY * dllMaterialiv )( GLenum face, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllMatrixMode )( GLenum mode );
extern	void ( APIENTRY * dllMultMatrixd )( const GLdouble* m );
extern	void ( APIENTRY * dllMultMatrixf )( const GLfloat* m );
extern	void ( APIENTRY * dllNewList )( GLuint list, GLenum mode );
extern	void ( APIENTRY * dllNormal3b )( GLbyte nx, GLbyte ny, GLbyte nz );
extern	void ( APIENTRY * dllNormal3bv )( const GLbyte* v );
extern	void ( APIENTRY * dllNormal3d )( GLdouble nx, GLdouble ny, GLdouble nz );
extern	void ( APIENTRY * dllNormal3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllNormal3f )( GLfloat nx, GLfloat ny, GLfloat nz );
extern	void ( APIENTRY * dllNormal3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllNormal3i )( GLint nx, GLint ny, GLint nz );
extern	void ( APIENTRY * dllNormal3iv )( const GLint* v );
extern	void ( APIENTRY * dllNormal3s )( GLshort nx, GLshort ny, GLshort nz );
extern	void ( APIENTRY * dllNormal3sv )( const GLshort* v );
extern	void ( APIENTRY * dllNormalPointer )( GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllOrtho )( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern	void ( APIENTRY * dllPassThrough )( GLfloat token );
extern	void ( APIENTRY * dllPixelMapfv )( GLenum map, GLsizei mapsize, const GLfloat* values );
extern	void ( APIENTRY * dllPixelMapuiv )( GLenum map, GLsizei mapsize, const GLuint* values );
extern	void ( APIENTRY * dllPixelMapusv )( GLenum map, GLsizei mapsize, const GLushort* values );
extern	void ( APIENTRY * dllPixelStoref )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllPixelStorei )( GLenum pname, GLint param );
extern	void ( APIENTRY * dllPixelTransferf )( GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllPixelTransferi )( GLenum pname, GLint param );
extern	void ( APIENTRY * dllPixelZoom )( GLfloat xfactor, GLfloat yfactor );
extern	void ( APIENTRY * dllPointSize )( GLfloat size );
extern	void ( APIENTRY * dllPolygonMode )( GLenum face, GLenum mode );
extern	void ( APIENTRY * dllPolygonOffset )( GLfloat factor, GLfloat units );
extern	void ( APIENTRY * dllPolygonStipple )( const GLubyte* mask );
extern	void ( APIENTRY * dllPopAttrib )( void );
extern	void ( APIENTRY * dllPopClientAttrib )( void );
extern	void ( APIENTRY * dllPopMatrix )( void );
extern	void ( APIENTRY * dllPopName )( void );
extern	void ( APIENTRY * dllPrioritizeTextures )( GLsizei n, const GLuint* textures, const GLclampf* priorities );
extern	void ( APIENTRY * dllPushAttrib )( GLbitfield mask );
extern	void ( APIENTRY * dllPushClientAttrib )( GLbitfield mask );
extern	void ( APIENTRY * dllPushMatrix )( void );
extern	void ( APIENTRY * dllPushName )( GLuint name );
extern	void ( APIENTRY * dllRasterPos2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * dllRasterPos2dv )( const GLdouble* v );
extern	void ( APIENTRY * dllRasterPos2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * dllRasterPos2fv )( const GLfloat* v );
extern	void ( APIENTRY * dllRasterPos2i )( GLint x, GLint y );
extern	void ( APIENTRY * dllRasterPos2iv )( const GLint* v );
extern	void ( APIENTRY * dllRasterPos2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * dllRasterPos2sv )( const GLshort* v );
extern	void ( APIENTRY * dllRasterPos3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllRasterPos3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllRasterPos3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllRasterPos3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllRasterPos3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * dllRasterPos3iv )( const GLint* v );
extern	void ( APIENTRY * dllRasterPos3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * dllRasterPos3sv )( const GLshort* v );
extern	void ( APIENTRY * dllRasterPos4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * dllRasterPos4dv )( const GLdouble* v );
extern	void ( APIENTRY * dllRasterPos4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * dllRasterPos4fv )( const GLfloat* v );
extern	void ( APIENTRY * dllRasterPos4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * dllRasterPos4iv )( const GLint* v );
extern	void ( APIENTRY * dllRasterPos4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * dllRasterPos4sv )( const GLshort* v );
extern	void ( APIENTRY * dllReadBuffer )( GLenum mode );
extern	void ( APIENTRY * dllReadPixels )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels );
extern	void ( APIENTRY * dllRectd )( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
extern	void ( APIENTRY * dllRectdv )( const GLdouble* v1, const GLdouble* v2 );
extern	void ( APIENTRY * dllRectf )( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
extern	void ( APIENTRY * dllRectfv )( const GLfloat* v1, const GLfloat* v2 );
extern	void ( APIENTRY * dllRecti )( GLint x1, GLint y1, GLint x2, GLint y2 );
extern	void ( APIENTRY * dllRectiv )( const GLint* v1, const GLint* v2 );
extern	void ( APIENTRY * dllRects )( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
extern	void ( APIENTRY * dllRectsv )( const GLshort* v1, const GLshort* v2 );
extern	GLint ( APIENTRY * dllRenderMode )( GLenum mode );
extern	void ( APIENTRY * dllRotated )( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllRotatef )( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllScaled )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllScalef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllScissor )( GLint x, GLint y, GLsizei width, GLsizei height );
extern	void ( APIENTRY * dllSelectBuffer )( GLsizei size, GLuint* buffer );
extern	void ( APIENTRY * dllShadeModel )( GLenum mode );
extern	void ( APIENTRY * dllStencilFunc )( GLenum func, GLint ref, GLuint mask );
extern	void ( APIENTRY * dllStencilMask )( GLuint mask );
extern	void ( APIENTRY * dllStencilOp )( GLenum fail, GLenum zfail, GLenum zpass );
extern	void ( APIENTRY * dllTexCoord1d )( GLdouble s );
extern	void ( APIENTRY * dllTexCoord1dv )( const GLdouble* v );
extern	void ( APIENTRY * dllTexCoord1f )( GLfloat s );
extern	void ( APIENTRY * dllTexCoord1fv )( const GLfloat* v );
extern	void ( APIENTRY * dllTexCoord1i )( GLint s );
extern	void ( APIENTRY * dllTexCoord1iv )( const GLint* v );
extern	void ( APIENTRY * dllTexCoord1s )( GLshort s );
extern	void ( APIENTRY * dllTexCoord1sv )( const GLshort* v );
extern	void ( APIENTRY * dllTexCoord2d )( GLdouble s, GLdouble t );
extern	void ( APIENTRY * dllTexCoord2dv )( const GLdouble* v );
extern	void ( APIENTRY * dllTexCoord2f )( GLfloat s, GLfloat t );
extern	void ( APIENTRY * dllTexCoord2fv )( const GLfloat* v );
extern	void ( APIENTRY * dllTexCoord2i )( GLint s, GLint t );
extern	void ( APIENTRY * dllTexCoord2iv )( const GLint* v );
extern	void ( APIENTRY * dllTexCoord2s )( GLshort s, GLshort t );
extern	void ( APIENTRY * dllTexCoord2sv )( const GLshort* v );
extern	void ( APIENTRY * dllTexCoord3d )( GLdouble s, GLdouble t, GLdouble r );
extern	void ( APIENTRY * dllTexCoord3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllTexCoord3f )( GLfloat s, GLfloat t, GLfloat r );
extern	void ( APIENTRY * dllTexCoord3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllTexCoord3i )( GLint s, GLint t, GLint r );
extern	void ( APIENTRY * dllTexCoord3iv )( const GLint* v );
extern	void ( APIENTRY * dllTexCoord3s )( GLshort s, GLshort t, GLshort r );
extern	void ( APIENTRY * dllTexCoord3sv )( const GLshort* v );
extern	void ( APIENTRY * dllTexCoord4d )( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
extern	void ( APIENTRY * dllTexCoord4dv )( const GLdouble* v );
extern	void ( APIENTRY * dllTexCoord4f )( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
extern	void ( APIENTRY * dllTexCoord4fv )( const GLfloat* v );
extern	void ( APIENTRY * dllTexCoord4i )( GLint s, GLint t, GLint r, GLint q );
extern	void ( APIENTRY * dllTexCoord4iv )( const GLint* v );
extern	void ( APIENTRY * dllTexCoord4s )( GLshort s, GLshort t, GLshort r, GLshort q );
extern	void ( APIENTRY * dllTexCoord4sv )( const GLshort* v );
extern	void ( APIENTRY * dllTexCoordPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllTexEnvf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllTexEnvfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllTexEnvi )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * dllTexEnviv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllTexGend )( GLenum coord, GLenum pname, GLdouble param );
extern	void ( APIENTRY * dllTexGendv )( GLenum coord, GLenum pname, const GLdouble* params );
extern	void ( APIENTRY * dllTexGenf )( GLenum coord, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllTexGenfv )( GLenum coord, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllTexGeni )( GLenum coord, GLenum pname, GLint param );
extern	void ( APIENTRY * dllTexGeniv )( GLenum coord, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexParameterf )( GLenum target, GLenum pname, GLfloat param );
extern	void ( APIENTRY * dllTexParameterfv )( GLenum target, GLenum pname, const GLfloat* params );
extern	void ( APIENTRY * dllTexParameteri )( GLenum target, GLenum pname, GLint param );
extern	void ( APIENTRY * dllTexParameteriv )( GLenum target, GLenum pname, const GLint* params );
extern	void ( APIENTRY * dllTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels );
extern	void ( APIENTRY * dllTranslated )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllTranslatef )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllVertex2d )( GLdouble x, GLdouble y );
extern	void ( APIENTRY * dllVertex2dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex2f )( GLfloat x, GLfloat y );
extern	void ( APIENTRY * dllVertex2fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex2i )( GLint x, GLint y );
extern	void ( APIENTRY * dllVertex2iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex2s )( GLshort x, GLshort y );
extern	void ( APIENTRY * dllVertex2sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertex3d )( GLdouble x, GLdouble y, GLdouble z );
extern	void ( APIENTRY * dllVertex3dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex3f )( GLfloat x, GLfloat y, GLfloat z );
extern	void ( APIENTRY * dllVertex3fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex3i )( GLint x, GLint y, GLint z );
extern	void ( APIENTRY * dllVertex3iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex3s )( GLshort x, GLshort y, GLshort z );
extern	void ( APIENTRY * dllVertex3sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertex4d )( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern	void ( APIENTRY * dllVertex4dv )( const GLdouble* v );
extern	void ( APIENTRY * dllVertex4f )( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern	void ( APIENTRY * dllVertex4fv )( const GLfloat* v );
extern	void ( APIENTRY * dllVertex4i )( GLint x, GLint y, GLint z, GLint w );
extern	void ( APIENTRY * dllVertex4iv )( const GLint* v );
extern	void ( APIENTRY * dllVertex4s )( GLshort x, GLshort y, GLshort z, GLshort w );
extern	void ( APIENTRY * dllVertex4sv )( const GLshort* v );
extern	void ( APIENTRY * dllVertexPointer )( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer );
extern	void ( APIENTRY * dllViewport )( GLint x, GLint y, GLsizei width, GLsizei height );
}

// Multitexture
#define    TEXTURE0_SGIS				0x835E
#define    TEXTURE1_SGIS				0x835F

typedef struct D3D_STATE_s
{
	int				wndWidth;
	int				wndHeight;
	HWND			hWnd;	// Window handle
	HDC				hDC;	// Device context
	void*			lpDD4;
	BOOL			bFullscreen;
	BOOL			f4444; // RGBA4444 format
	int				textureLayer;
	D3DCOLOR		color;
	D3DCOLOR		clearColor;
	const GLvoid*	colorPointer;
} D3D_STATE_t;

#endif // _OPENGL32_H_