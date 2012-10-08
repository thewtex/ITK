FILE(GLOB_RECURSE CoreCommonHeadersList *.h)
add_custom_target(CoreCommonHeadersSources SOURCES
${CoreCommonHeadersList}
)

FILE(GLOB_RECURSE CoreCommonImplementationsList *.hxx)
add_custom_target(CoreCommonImplementationsSources SOURCES
${CoreCommonImplementationsList}
)
