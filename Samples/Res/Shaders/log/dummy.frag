Shaders/src/dummy.frag
Warning, version 450 is not yet complete; most version-specific features are present, but some are missing.

// Module Version 10000
// Generated by (magic number): 80001
// Id's are bound by 22

                              Capability Shader
               1:             ExtInstImport  "GLSL.std.450"
                              MemoryModel Logical GLSL450
                              EntryPoint Fragment 4  "main" 9 19
                              ExecutionMode 4 OriginUpperLeft
                              Source GLSL 450
                              Name 4  "main"
                              Name 9  "o_Color"
                              Name 10  "UniformBufferObject"
                              MemberName 10(UniformBufferObject) 0  "colour"
                              Name 12  "ubo"
                              Name 19  "v_Color"
                              Decorate 9(o_Color) Location 0
                              MemberDecorate 10(UniformBufferObject) 0 Offset 0
                              Decorate 10(UniformBufferObject) Block
                              Decorate 12(ubo) DescriptorSet 0
                              Decorate 12(ubo) Binding 0
                              Decorate 19(v_Color) Location 0
               2:             TypeVoid
               3:             TypeFunction 2
               6:             TypeFloat 32
               7:             TypeVector 6(float) 4
               8:             TypePointer Output 7(fvec4)
      9(o_Color):      8(ptr) Variable Output
10(UniformBufferObject):             TypeStruct 7(fvec4)
              11:             TypePointer Uniform 10(UniformBufferObject)
         12(ubo):     11(ptr) Variable Uniform
              13:             TypeInt 32 1
              14:     13(int) Constant 0
              15:             TypePointer Uniform 7(fvec4)
              18:             TypePointer Input 7(fvec4)
     19(v_Color):     18(ptr) Variable Input
         4(main):           2 Function None 3
               5:             Label
              16:     15(ptr) AccessChain 12(ubo) 14
              17:    7(fvec4) Load 16
              20:    7(fvec4) Load 19(v_Color)
              21:    7(fvec4) FMul 17 20
                              Store 9(o_Color) 21
                              Return
                              FunctionEnd