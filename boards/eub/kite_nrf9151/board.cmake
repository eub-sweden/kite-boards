# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BOARD_KITE_NRF9151_NRF9151_NS)
  set(TFM_PUBLIC_KEY_FORMAT "full")
endif()

if(CONFIG_TFM_FLASH_MERGED_BINARY)
  set_property(TARGET runners_yaml_props_target PROPERTY hex_file tfm_merged.hex)
endif()

# FIXME: Does this even work
board_runner_args(jlink "--device=nRF9151_xxCA" "--speed=4000")
board_runner_args(pyocd "--target=nrf9160_xxaa" "--frequency=4000000")
include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
