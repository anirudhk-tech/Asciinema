# Contributing

Thanks for your interest in contributing. This project aims to stay small, readable, and performance-focused.

## Development setup

### Dependencies

- CMake
- OpenCV 4.x
- ncurses

### Build

```bash
make build
```

### Run

```bash
make run VIDEO=samples/test.mp4
make run VIDEO=samples/test.mp4 COLOR=1
make run VIDEO=samples/test.mp4 COLOR=1 BP=1
```

## Code style

- C++17
- Prefer RAII and value semantics
- Avoid global state (except minimal signal wiring)
- Keep hot paths allocation-free where reasonable (avoid per-frame heap churn)
- Format with:

```bash
make format
```

## Submitting changes

1. Create a branch
2. Make focused commits with clear messages
3. Ensure `make build` succeeds
4. Open a PR with:
   - What changed
   - Why it changed (performance, correctness, UX)
   - How to validate (commands + expected behavior)

## Reporting issues

When filing a bug report, include:

- OS + terminal emulator
- Build toolchain (compiler version)
- Run command + flags
- Sample video characteristics (codec, resolution, FPS)
- A screenshot or recording if relevant
