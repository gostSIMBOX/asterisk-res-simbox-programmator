# asterisk-res-simbox-programmator

SimBox firmware and modem-programming support delivered as an Asterisk module
and as a standalone operator tool.

## Artifacts

- `res_simbox_programmator.so` — Asterisk CLI module;
- `programmator` — standalone firmware flashing executable.

The two artifacts are independent entry points over the migrated legacy
programming code. The executable is intended for explicit end-user/operator
use; Asterisk and `res_simbox_core.so` never start it.

## Dependencies

For `res_simbox_programmator.so`:

- Asterisk headers and target runtime;
- `asterisk-res-simbox-core` source headers at build time;
- a loaded `res_simbox_core.so` at runtime for device state and helper symbols.

For `programmator`:

- Linux serial/TTY APIs;
- write access to `/var/lock`;
- an existing writable `/var/svistok/devices/<device>/` state directory.

## Build

Build both artifacts:

```sh
make ASTERISK_INCLUDE=/path/to/asterisk/includes \
     DONGLE_CORE_INCLUDE=/path/containing/asterisk-res-simbox-core
```

Remove outputs with:

```sh
make clean
```

## Standalone use

```text
programmator <tty-device> <device-id> <firmware-file>
```

Example:

```sh
programmator /dev/ttyUSB5 3-1.1.1 firmware.bin
```

The tool switches the device to diagnostic mode, waits for it, uploads the
firmware and records state/progress below
`/var/svistok/devices/<device-id>/`. It operates on hardware and should only be
run with a verified firmware image and exclusive access to the TTY.

## Asterisk use

Install both core and programmator modules in the Asterisk modules directory,
then load core before programmator:

```text
module load res_simbox_core.so
module load res_simbox_programmator.so
```

The module adds:

```text
dongle diagmode <device>
dongle changeimei <device> <new-imei>
dongle update
```

The CLI module uses live device state owned by core. It is not a wrapper around
the standalone `programmator` process.

## Verification status

The artifact split and dependency direction are covered by source-level tests
in [`flows/sdd-res-simbox`](../../flows/sdd-res-simbox/). Compilation, module
loading and firmware transfer still require validation on the target
Linux/Asterisk and modem hardware.

## License

See [LICENSE](LICENSE).
