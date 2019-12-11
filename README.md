# FixProtocolParser

[Ссылка](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/Task/test_task.txt "") на задание.  
[Ссылка](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/Task/file1.txt "") на входные данные.  

[Ссылка](https://drive.google.com/open?id=1ciq35A6YafTgssW7KL3akY6BFYN4Ga7c "") на решение.  

## Использовалось
c++14. GCC 7.4.0. IDE Qt Creator. Qt 5.9.5. OS Ubuntu 18.04.  

## О решениях
### Реализация парсера
При выполнении задачи я находился в "творческом поиске", поэтому позволил себе немного поэксперементировать. Так, например, в реализации парсера я использовал паттерн Декоратор. Сначала он показался мне уместным, но позже, в процессе реализации, я понял, что он тут не совсем к месту.  
Выбирая данный патерн, я хотел инкапсулировать часть логики парсинга в отдельные блоки, которые, позже, можно было бы использовать для упрощения дальнейшего расширения парсера и чтения прочих данных, а также легко добавлять новые подобные блоки.  

### Сторонние API для парсинга
В начале работы я немного поигрался с [OnixS](https://www.onixs.biz/ "") (одна из ссылок источников вела на их сайт, потому заинтересовало), используя которую можно было бы достаточно легко распарсить файл, однако, исходя из [ТЗ](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/Task/test_task.txt ""), а конкретно из фразы "разделителем полей является 0x01, в файле с примером данных в качестве разделителя используется символ '^'", я решил, что использование сторонних API заданием не предусмотрено. Замена разделителя `0x01` на `^` мною была расценена как намеренная помеха, целью которой было помешать использование посторонних API (насколько я понял, `OnixS` съедает только сообщения, которые используют в качестве разделителя `0x01`).  
В ходе изучения `OnixS` эта помеха мною была решена, и проблем с парсингом у меня не возникло, так что, на случай, если использование сторонних API всё же было разрешено, то для себя я с этой задачей справился.  

### Путь к файлу  
В [ТЗ](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/Task/test_task.txt "") не сказано ни слова относительно способа получения пути к входному файлу.  
Для простоты, в `debug` сборке, парсится входной файл с именем `file1.txt`, расположенный в [каталоге](https://github.com/AndrewRuzavin/FixProtocolParser/tree/master/Task "") репозитория Task.  
В `release` парсится входной файл с именем `file1.txt`, расположенный непосредственно рядом с выполняемым файлом (в примерах приводится именно такой вариант).  
![image](https://user-images.githubusercontent.com/32496224/70574103-59084980-1bb4-11ea-8b36-b09ae0375bdb.png)  
Путь зашит в [файле](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/config.pri "").  

### Используемые поля сообщений
В ходе изучения документации протокола FIX, сложилось стойкое впечатление, что я не до конца понял значение некоторых полей, поэтому допускаю, что при парсинге ошибся, потому решил описать упрощённый алгоритм чтения записей (без определения конца записи по проверочной сумме и т.д.), чтобы показать, как я вижу считываемый лог.  
Насколько я понял, задача заключалась в реализации следующих шагов:  
1) Найти запись, тег `35` (тип сообщения) которой был бы равен `X` "Incremental Refresh".  
2) Найти группу записей, начинающуюся с тега `268` (количество записей в группе) и пройтись по текущему сообщению `n` раз, где `n` - значение этого тега.  
3) Считать тег `279` (тип обновления) для каждой группы. Мною использовался исключительно как разделитель между группами записей.  
Перебрал несколько источников, но не понял, как использовать значения данного тега, и нужно ли вообще его использовать. Книга отображается в консоли по окончанию парсинга, соответственно все изменения (0 - добавление, 1 - изменение, 2 - удаление), выполняемые над записью, нас не интересуют. Если не прав, хотел бы уточнить этот момент.  
4) Считать тег `269` (сторона), определяющий, к кому относится эта запись - к покупателю (0 - bid), продавцу (1 - ask) или третья сторона (2 - trade), значения которой я не понял. Записи, чей тег `269` равнялся 2 (trade) мною не учитывались.
5) Считать теги `270` (цена за единицу товара со стороны, на которую указывает тег `269`) и `271` (количество единиц товара по данной цене со стороны, на которую указывает тег `269`).  
6) Запомнить и, позже, вывести данные, полученные из тегов `269` (определяет, с какой стороны будет выведена запись) `270` (цена) `271` (количество единиц).  

Насколько я понял, записи, чей тег `35` равен `W` ("Snapshot") и `V` ("Request"), парсить не требовалось, т.к они не содержат полезной (для конкретного задания) информации.  
Не совсем понял значение тега `100090`, определяющей цену символа. Полагаю, что это некоторое средняя цена за единицу товара.  

### Максимальное количество выводимых записей
Значение зашито в коде и равно `5`. В [ТЗ](https://github.com/AndrewRuzavin/FixProtocolParser/blob/master/Task/test_task.txt "") не сказано, что это значение обязательно нужно вводить с клавиатуры, поэтому мною было решено оставить такой вариант.  
В случае, если реальный размер любой из сторон книги меньше максимального значения, вывод будет произведён корректно (см. пример).  

### Тестирование
Если посмотреть коммиты, можно увидеть, что изначально проект состоял из двух проектов - основного и тестового, но, в результате, тестовый проект был убран. На текущий момент я нахожусь в процессе изучения тестрования и, при выполнении текущего задания, до него попросту не дошли руки. Кроме того, пока не успел освоить моки, и не уверен как в целом тестировать парсер.  

## Пример выполнения
![image](https://user-images.githubusercontent.com/32496224/70572927-d8484e00-1bb1-11ea-9ce3-432be1d01e39.png)
В процессе парсинга все записи, размер которых был равен 0 отбасывался, потому, в итоговом отображении они отсутствуют.
Максимальное выводимое количество сообщений  

![image](https://user-images.githubusercontent.com/32496224/70573199-7f2cea00-1bb2-11ea-8728-a8d87c29124f.png)  
Парсер способен обрабатывать малое количество записей.  

![image](https://user-images.githubusercontent.com/32496224/70572985-0037b180-1bb2-11ea-993e-c5d2eeb7dfe8.png)  
Вывод при отсутствии входного файла.  

## Источники
Базовое описание протокола [FIX](https://en.wikipedia.org/wiki/Financial_Information_eXchange "").  
[Описание](https://docs.deribit.com/v2/#market-data-snapshot-full-refresh-w "") схем сообщений снапшота.  
[Описание](https://docs.deribit.com/v2/#market-data-incremental-refresh-x "") схем сообщений инкрементов.  
[Пример](https://ref.onixs.biz/cpp-fix-engine-guide/group__fix-protocol-repeating-group.html "") группы полей.  
[Описание](https://fxssi.com/stock-exchange-order-book-exist-forex-market "") того, что такое стакан (OrderBook).  
