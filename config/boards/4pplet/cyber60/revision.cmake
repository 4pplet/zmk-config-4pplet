# SPDX-License-Identifier: MIT

set(BOARD_REVISIONS "rev_a" "rev_b" "rev_c" "rev_d" "rev_e")
if(NOT DEFINED BOARD_REVISION)
  set(BOARD_REVISION "rev_e")
else()
  if(NOT BOARD_REVISION IN_LIST BOARD_REVISIONS)
    message(FATAL_ERROR "${BOARD_REVISION} is not a valid revision for cyber60. Accepted revisions: ${BOARD_REVISIONS}")
  endif()
endif()
