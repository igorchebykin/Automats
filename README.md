Перевод НКА -> 0-1 НКА -> ДКА
 
 BadAutomat - НКА с произвольными переходами

 Automat - 0-1 НКА

 DeterminedAutomat - ДКА

Запуск:
cmake -S . -B build
cmake --build build

Для запуска тестов:
cmake --build build --target run_tests

Для запуска main:
cmake --build build --target run_main