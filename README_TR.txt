GameDealFinder

Basit bir C++ oyun fiyat karşılaştırma programı  
Oyun fiyatlarını farklı platformlarda karşılaştırır ve en uygun olanı gösterir.  
Toplu ekleme/güncelleme, isme göre silme (onaylı), ve fiyat güncelleme özelliklerine sahiptir.

---

Özellikler

- Oyunları tam veya kısmi isme göre arama  
- Tüm mevcut oyunları listeleme  
- GameName;Platform;Price formatıyla toplu oyun ekleme veya güncelleme  
- Oyunları isme göre silme, tek tek veya topluca (onaylı)  
- Mevcut bir oyunun fiyatını güncelleme  
- Tüm veriler her platform için ayrı `.txt` dosyalarında saklanır

---

Kurulum (Windows)

Not: İlk kez çalıştırmadan önce programı Yönetici olarak çalıştırmanız önerilir.

1. `GameDealFinder_Setup.exe` dosyasını çalıştırın  
2. Kurulum sihirbazını takip edin  
3. Programı Başlat menüsünden veya Masaüstü'nden başlatın

---

Kullanım

Programı başlattığınızda aşağıdaki menü görünür:

1) Search for a game
2) List all available games
3) Bulk add
4) Delete a game
5) Update the price of a game
6) Exit

---

Bulk Add/Update

3 numaralı seçenekle birden fazla oyunu şu formatla ekleyin:

GTA V;steam;29.99  
The Witcher 3;gog;49.99  
Cyberpunk 2077;epic;59.99  
Hades;steam;21.99  

- Girişleri bitirmek için boş bir satır bırakın  
- Aynı platformda var olan bir oyun varsa, fiyatı güncellenir

---

Delete Games

- Oyun adının bir kısmını girin (örn: "grand")  
- Eşleşen tüm oyunlar listelenir  
- Şunları yapabilirsiniz:
  - Belirli oyunları seçerek silebilirsiniz  
  - Sileceğiniz platformları seçebilirsiniz  
  - Ya da `0` girerek tüm eşleşenleri topluca silebilirsiniz  
- Tümünü silme seçeneği, işlem öncesinde onay ister

---

Veri

- Tüm oyun verileri programla aynı dizindeki `data/` klasöründe saklanır  
- Her platformun ayrı `.txt` dosyası vardır:
  - steam.txt  
  - epic.txt  
  - gog.txt  
  - microsoft.txt

---

Geliştirici

Bu proje **Hüseyin Can Çaltı** tarafından geliştirilmiştir  
Web Sitesi: https://huseyincancalti.github.io/karakedidub/
