Dzia�anie sterownika temperatury polega na sterowniu prac� pompek zapewniaj�cych wymian� ciep�a mi�dzy zasobnikiem a solarem i piecem. Dodatkowo w projekcie znajduje si� 
pompka od cyrkulacji wody, b�d�cej w obiegu mi�dzy poszczeg�lymi punktami w domu, gdzie pobierana jest ciep�a woda. W sk�ad sterownika wchodz� 3 czujniki rezystancyjne
PT100, kt�re mierz� temperatur� w piecu, solarze i zasobniku. Temperatura wody w obiegu mierzona jest czujnikiem tc74a0 (i2c). Na wy�wietlaczu TFT LCD urz�dzenia widoczne s� wskazania ze
wszystkich czujnik�w, na ich podstawie mierzone s� r�nice temperatur i wyliczane jest wype�nienie sygna�u PWM steruj�cego prac� pompek a tak�e spodziewane napi�cie na pompkach. Dodatkowo
sterownik posiada menu, w kt�rym mo�na zmienia� r�nice temperatur, przy kt�rych ma nast�pi� za��czanie poszczeg�lnych pompek. Dodatkowo mo�na ustawi� aktualny czas (zapisywany
wewn�trz RTC mikrokontrolera). 