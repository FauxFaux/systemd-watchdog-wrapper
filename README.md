`systemd-watchdog-wrapper-example.service`: A service which runs the wrapper and
the watchdog directly. Easy to understand and works. Breaks `Type=notify` from
services, and things with built-in Watchdog support (probably).

`foo.service` and `foo-monitor.service`: An attempt to have an external monitor
service. It works, in that when the healthcheck fails, `foo.service`. gets stopped.
However, `Restart=always` does not cause `foo.service` to get restarted, so there's
clearly an issue here. I believe it should be possible to do this with a shell script,
but `systemd-notify --pid=$$ WATCHDOG=1` seems to fail horribly: `systemd` never
manages to associate the calls with the service, even with `NotifyAccess=all`.

### Building

```
apt install build-essential libsystemd-dev
autoreconf -fvi
./configure
make
src/systemd-watchdog-wrapper
```

