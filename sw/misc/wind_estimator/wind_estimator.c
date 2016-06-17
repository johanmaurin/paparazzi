/*
 * Copyright (C) 2016 Felix Ruess <felix.ruess@gmail.com>
 *
 * This file is part of Paparazzi.
 *
 * Paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * Paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file examples/C/linux_uart_example.c
 *
 * Example on how to use pprzlink with pprz_transport on Linux over a UART like device.
 */

#include "linux_uart_pprz_transport.h"

// for telemetry messages
#include "pprzlink/messages.h"
// for datalink messages
#include "pprzlink/dl_protocol.h"

#include <unistd.h>

#define SENDER_AC_ID 0

uint8_t buffer[256];
bool msg_available;

void do_stuff(void);
void do_stuff(void)
{
  pprz_check_and_parse(&link_dev, &pprz_tp, buffer, &msg_available);

  if (msg_available) {
    if (buffer[0] == PPRZ_MSG_ID_PAYLOAD_FLOAT) {
      uint8_t dest_id = buffer[1];
      // extract data
      // start estimation
      // send result
      uint8_t flags = 3; // update 3D wind
      float airspeed = 0.f;
      pprz_msg_send_WIND_INFO(&pprz_tp.trans_tx, &link_dev, SENDER_AC_ID, &dest_id, ..., &airspeed);
    }
    msg_available = false;
  }
}


int main(void)
{
  msg_available = false;
  pprz_link_init();
  init_uart();

  message_from_uart();
  while (1) {
    usleep(1000);
    do_stuff();
  }
}
