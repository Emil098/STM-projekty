Dzia³anie sterownika temperatury polega na sterowniu prac¹ pompek zapewniaj¹cych wymianê ciep³a miêdzy zasobnikiem a solarem i piecem. Dodatkowo w projekcie znajduje siê 
pompka od cyrkulacji wody, bêd¹cej w obiegu miêdzy poszczególymi punktami w domu, gdzie pobierana jest ciep³a woda. W sk³ad sterownika wchodz¹ 3 czujniki rezystancyjne
PT100, które mierz¹ temperaturê w piecu, solarze i zasobniku. Temperatura wody w obiegu mierzona jest czujnikiem tc74a0 (i2c). Na wyœwietlaczu TFT LCD urz¹dzenia widoczne s¹ wskazania ze
wszystkich czujników, na ich podstawie mierzone s¹ ró¿nice temperatur i wyliczane jest wype³nienie sygna³u PWM steruj¹cego prac¹ pompek a tak¿e spodziewane napiêcie na pompkach. Dodatkowo
sterownik posiada menu, w którym mo¿na zmieniaæ ró¿nice temperatur, przy których ma nast¹piæ za³¹czanie poszczególnych pompek. Dodatkowo mo¿na ustawiæ aktualny czas (zapisywany
wewn¹trz RTC mikrokontrolera). 