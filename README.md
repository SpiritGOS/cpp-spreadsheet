# Дипломный проект: Электронная таблица
Электронная таблица, работа с которой осуществляется программно без использования графического интерфейса (Сильно упрощенный аналог существующих решений: лист таблицы Microsoft Excel или Google Sheets.). Проект предусматривает написание сценариев на выбранном языке программирования для работы с данными, их анализа и создания отчетов. 
## Инструкция по развёртыванию
1. Для начала необходимо скопировать репозиторий. Например командой:
   
   ```
   git clone https://github.com/SpiritGOS/cpp-spreadsheet.git
   ```
2. Далее необходимо установить ANTLR и скачать antlr4-cpp-runtime-4.13.1-source (Версия может отличаться). Файлы для установки и инструкции можно найти на официаотном сайте http://www.antlr.org/
3. antlr4-cpp-runtime-4.13.1-source.zip нужно распаковать в директорию к остальным .cpp и .h файлам. antlr-4.13.1-complete.jar необходимо поместить там же.
4. Если версия ANTLR отличается от указанной в инструкции - в CMakeLists.txt замените версию JAR-файла на актуальную.
5. Создаем файлы сборки CMake (для файлов сгенерированных CMake желательно создать отдельную директорию, например build, и запускать команду оттуда):

   ```
   cmake ../spreadsheet/
   ```
6. В Данном проекте обращение к таблице происходит в коде, поэтому дописываем все необходимые действия с таблицей в main().
7. Собираем проект:

   ```
   cmake --build .
   ```
8. Готово!
## Инструкция по использованию
Для создания таблицы есть функция CreateSheet() определенная в файле [sheet.cpp](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L101)
Взаимодействие с таблицей происходит через методы:
1. [SetCell()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L14) - Установки значения в ячейку
2. [GetCell()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L32) - Получение значение ячейки
3. [ClearCell()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L51) - Очистка ячейки
4. [GetPrintableSize()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L61) - Получение размера таблицы
5. [PrintValues()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L77) - Вывод не строковых ячеек таблицы
6. [PrintTexts()](https://github.com/SpiritGOS/cpp-spreadsheet/blob/main/spreadsheet/sheet.cpp#L90) - Вывод строковых ячеек тпблицы
## Требования
- C++17
- ANTLR (не старше версии 4.13.1)

![cat](https://github.com/SpiritGOS/cpp-spreadsheet/assets/62353945/1c7cc962-e8cb-4086-83ff-32152404aa8d)
