Shaders/src/dummy.vert
// Module Version 10000
// Generated by (magic number): 80001
// Id's are bound by 21

                              Capability Shader
               1:             ExtInstImport  "GLSL.std.450"
                              MemoryModel Logical GLSL450
                              EntryPoint Vertex 4  "main" 10 14 18 19
                              Source GLSL 450
                              Name 4  "main"
                              Name 8  "gl_PerVertex"
                              MemberName 8(gl_PerVertex) 0  "gl_Position"
                              Name 10  ""
                              Name 14  "i_Position"
                              Name 18  "v_Color"
                              Name 19  "i_Color"
                              MemberDecorate 8(gl_PerVertex) 0 BuiltIn Position
                              Decorate 8(gl_PerVertex) Block
                              Decorate 14(i_Position) Location 0
                              Decorate 18(v_Color) Location 0
                              Decorate 19(i_Color) Location 1
               2:             TypeVoid
               3:             TypeFunction 2
               6:             TypeFloat 32
               7:             TypeVector 6(float) 4
 8(gl_PerVertex):             TypeStruct 7(fvec4)
               9:             TypePointer Output 8(gl_PerVertex)
              10:      9(ptr) Variable Output
              11:             TypeInt 32 1
              12:     11(int) Constant 0
              13:             TypePointer Input 7(fvec4)
  14(i_Position):     13(ptr) Variable Input
              16:             TypePointer Output 7(fvec4)
     18(v_Color):     16(ptr) Variable Output
     19(i_Color):     13(ptr) Variable Input
         4(main):           2 Function None 3
               5:             Label
              15:    7(fvec4) Load 14(i_Position)
              17:     16(ptr) AccessChain 10 12
                              Store 17 15
              20:    7(fvec4) Load 19(i_Color)
                              Store 18(v_Color) 20
                              Return
                              FunctionEnd
