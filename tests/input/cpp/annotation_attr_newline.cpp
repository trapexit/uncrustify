[[nodiscard]] int build_value();
[[nodiscard]] [[maybe_unused]] int maybe_value();

void consume(int tag)
{
switch (tag) {
case 1: [[fallthrough]]; case 2: break;
}
}
