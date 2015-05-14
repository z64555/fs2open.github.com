
include(WriteCompilerDetectionHeader)

set(REQUIRED_FEATURES cxx_rvalue_references)
set(OPTIONAL_FEATURES cxx_variadic_templates)

target_compile_features(code PUBLIC ${REQUIRED_FEATURES})

write_compiler_detection_header(
  FILE "${GENERATED_SOURCE_DIR}/code/scp_compiler_detection.h"
  PREFIX SCP
  COMPILERS AppleClang Clang GNU MSVC
  FEATURES ${OPTIONAL_FEATURES}
)
