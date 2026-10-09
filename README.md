Перевод НКА -> 0-1 НКА -> ДКА
 
 BadAutomat - НКА с произвольными переходами

 Automat - 0-1 НКА

 DeterminedAutomat - ДКА

EPSILON = 1

1 не может быть в алфавите

Запуск:
cmake -S . -B build

cmake --build build

Для запуска тестов:
cmake --build build --target run_tests

Для запуска main:
cmake --build build --target run_main