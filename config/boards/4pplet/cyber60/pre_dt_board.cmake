# SPDX-License-Identifier: MIT

# Suppress "unique_unit_address_if_enabled" warnings
list(APPEND EXTRA_DTC_FLAGS "-Wno-unique_unit_address_if_enabled")
list(APPEND EXTRA_DTC_FLAGS "-Wno-simple-bus-reg")
