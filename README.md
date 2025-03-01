# TODO
- na ten moment aplikacja nie zadziala ze wzgledu na brak dostarczonych modeli(ograniczenie GitHuba modele za dużo ważyły)
- optymalizacja aplikacji
- zmiana struktury
- "wyczyszczenie" kodu
- zmiana sposobu generowania modeli
# InteriorDesigner
Prosty program do umieszczania obiektów w scenerii, wykorzystujący język c++ razem z OpenGL
# Krótki opis aplikacji
Aplikacja wykorzystuje bibliotekę ASSIMP do wczytywania i wyświetlania obiektów. Wszystkie obiekty są zapisane w standardzie .fbx, ze względu na ich mniejszy rozmiar od standardowego .obj. Dane te są przekazywane do OpenGL w celu przetworzenia informacji o obiekcie i generowania grafiki trójwymiarowej
# Wymagania sprzętowe
System operacyjny: od Windows 10 w górę
Pamięć: ok 200 MB
Komputer na którym testowana była aplikacja:
-Karta Graficzna: Nvidia GTX 1050 Ti 4GB
-Procesor: AMD Ryzen 5 1600
-Pamięć RAM: 24GB
# Instalacja całego projektu
Program tworzony był od podstaw w Visual Studio 2022 oraz wykorzsytuje biblioteki Windowsa (np windows.h) dlatego program nie będzie działał na innych systemach operacyjnych
1. Pobieramy Visual Studio razem z pakietami do C++ (znajdują się tam wszelkie biblioteki oraz kompilatory do uruchamiania kodu)
2. Uruchamiamy Visual Studio
3. Wybieramy opcje "Sklonuj repozytorium"
4. Wprowadzamy link do projektu: https://github.com/downloadfreeram/3DInteriorDesigner/
5. Po pobraniu repozytorium budujemy rozwiązanie (CTRL + SHIFT + B)
6. Po zakończeniu możemy uruchomić program w trybie Debug klawiszem F5
# Instalacja aplikacji dla użytkownika
1. Pobieramy pakiet Microsoft Visual C++ Redistributable 2015-2022
2. Ściągamy repozytorium za pomocą git (git clone https://github.com/downloadfreeram/3DInteriorDesigner.git) lub poprzez github: https://github.com/downloadfreeram/3DInteriorDesigner/
3. Wchodzimy w folder główny i uruchamiamy plik wykonywalny 3DInteriorDesigner.exe
# Sterowanie
WASD - poruszanie się kamerą
LMB + Mysz - obracanie kamerą
Q - zapisywanie
R - otwarcie okna z obiektami
# WAŻNE
Program posiada funkcje zapisu i odczytu pliku. Jest to plik binarny o rozszerzeniu .bin, który należy umiesczać bezpośrednio w folderze aplikacji. Umieszczony plik gdziekolwiek indziej, nie będzie w stanie poprawnie się wczytać.
