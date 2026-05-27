# Reads patrons.json and generates PatreonData.hpp with the content embedded
# as a C++ raw string literal. Runs at build time via add_custom_command.

file(READ "${SRC_DIR}/patrons.json" _json)
file(WRITE "${OUTPUT_FILE}"
  "#pragma once\n"
  "namespace UI {\n"
  "inline constexpr const char* EMBEDDED_PATRONS_JSON = R\"-JSON-(\n"
  "${_json}\n"
  ")-JSON-\";\n"
  "}\n"
)
