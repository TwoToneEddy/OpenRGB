# Sapphire NITRO+ B850M WIFI ARGB

The `Sapphire NITRO+ B850M WIFI` detector supports the board whose DMI vendor is
`SAPPHIRE` and board name is `SAPPHIRE NITRO+ B850M WIFI`. Detection is limited to
AMD FCH SMBus (`1022:790b`), port 0, address `0x29`. It only reads registers during
detection and initialization. Other Sapphire boards are not enabled by this change.

The board exposes one shared **ARGB Headers** zone. Static colour, brightness,
Rainbow, Breathing, and Off are supported. Rainbow and Breathing have speed
control. Individual fan LEDs and independent header colours are not exposed;
TriXX-M also treats this model's headers as a single group. There is no explicit
hardware-save operation or per-LED Direct mode.

## Use

After building OpenRGB, select **SAPPHIRE NITRO+ B850M WIFI**, select **Static**,
and choose a colour. For example:

```sh
./openrgb --device 'SAPPHIRE NITRO+ B850M WIFI' --mode Static --color 0080FF --brightness 50
```

Brightness and speed on the CLI are percentages. Linux needs `i2c-dev` and
`i2c-piix4`, and read/write access to the motherboard's `/dev/i2c-*` device.
See [SMBus access](SMBusAccess.md). This implementation has been built and tested
on Linux; the detector requires primary-port metadata (`port_id == 0`). Other
platforms have not been validated.

For the Bazzite development build in this checkout, `build/run-openrgb` launches
`build/openrgb` with its bundled mbedTLS libraries. `build/sapphire-config` is a
separate configuration enabling only this motherboard detector:

```sh
./build/run-openrgb --config ./build/sapphire-config --noautoconnect --gui
./build/run-openrgb --config ./build/sapphire-config --noautoconnect \
    --device 'SAPPHIRE NITRO+ B850M WIFI' --mode Static --color 0080FF
```

These build artifacts are local and ignored by Git. They do not replace an
installed OpenRGB or its configuration.

## Protocol evidence

The command definitions were established by inspecting the lighting control
path in Sapphire's official [TriXX-M 3.3.5 package](https://www.sapphiretech.com/en/software-mb)
(`TRIXX.V3.3.5s.exe`, SHA-256
`4b0fd76e32bab10f276613fd89822124dd21c70b046e6e8448358e0fc2f29034`).
No vendor program code or binaries are included in the driver.

TriXX-M addresses the lighting MCU through AMD SMBus at base `0x0B00`, slave
address `0x29`, using SMBus byte-data transactions. The attached USB device
`2e82:7005` is not used by this driver. On the tested board, the relevant bus was
`SMBus PIIX4 adapter port 0 at 0b00` (`/dev/i2c-7`); the bus number is not hardcoded.

| Register | Meaning | Values used |
| --- | --- | --- |
| `0x10` | Mode / apply colour | Rainbow `0`, Static `6`, Off `7`, Breathing `8` |
| `0x15` | Rainbow speed | `9` slowest through `0` fastest |
| `0x1A` | Red | `0`–`255` |
| `0x1B` | Green | `0`–`255` |
| `0x1C` | Blue | `0`–`255` |
| `0x1D` | Breathing speed | `9` slowest through `0` fastest |
| `0x3E` | Brightness | `0`–`10` |

Colour writes set R, G, B, then reapply the current colour-capable mode. Writes
are spaced by 5 ms, matching the native library's post-write delay. A failed
write stops the sequence and logs a warning. Unsupported modes and out-of-range
brightness/speed values are rejected without writes. No firmware update,
partition-selection, or LED-count commands are sent.

## Validation

On a SAPPHIRE NITRO+ B850M WIFI with board version `AM50L301R29`:

- Full Qt 5 Linux application build succeeded.
- The built application detected the motherboard and its shared zone.
- The owner confirmed that a blue colour test changed the fan lighting.
- CLI mode changes to Static, Rainbow, Breathing, and Off read back correctly.
- CLI colour `123456` and 50% brightness read back as R=`0x12`, G=`0x34`,
  B=`0x56`, brightness=`5`.
- Original colour, brightness, and mode were restored and verified afterward.

Transport tests use a mock SMBus to check channel order, mode application,
boundaries, and abort-on-error behavior without accessing hardware:

```sh
sh tests/SapphireMotherboardController/run.sh
```
