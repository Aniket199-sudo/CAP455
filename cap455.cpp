#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;
 
struct FileRecord
{
    int id;
    char name[50];
    float price;
    float discount;
    float salePrice;
    char category;
};
 
class Item
{
private:
    int id;
    string name;
    float price;
    float discount;
    float salePrice;
    char category;
 
    static int cnt;
 
public:
    Item()
    {
        id = 0;
        name = "";
        price = 0;
        discount = 0;
        salePrice = 0;
        category = 'N';
    }
 
    Item(int x, string y, float p, float d, char c)
    {
        id = x;
        name = y;
        price = p;
        discount = d;
        salePrice = p - (p * d / 100);
        category = c;
    }
 
    Item(const Item &temp)
    {
        id = temp.id;
        name = temp.name;
        price = temp.price;
        discount = temp.discount;
        salePrice = temp.salePrice;
        category = temp.category;
    }
 
    ~Item()
    {
    }
 
    void setData(int &x, string &y, float &p, float &d, char &c)
    {
        this->id = x;
        this->name = y;
        this->price = p;
        this->discount = d;
        this->category = c;
        this->salePrice = p - (p * d / 100);
        cnt++;
    }
 
    void display()
    {
        cout << id << "\t" << name << "\t" << price << "\t"
             << discount << "%\t" << salePrice << "\t" << category << endl;
    }
 
    inline int getId()
    {
        return id;
    }
 
    float getSalePrice()
    {
        return salePrice;
    }
 
    static int getCount()
    {
        return cnt;
    }
 
    static void setCount(int n)
    {
        cnt = n;
    }
 
    friend void showCount();
};
 
int Item::cnt = 0;
 
void showCount()
{
    cout << "\nTotal Records: " << Item::cnt << endl;
}
 
bool idExists(int id)
{
    fstream file("data/records.dat", ios::binary | ios::in);
    if (!file)
    {
        return false;
    }
 
    FileRecord rec;
    long pos = 0;
 
    while (true)
    {
        file.seekg(pos * sizeof(FileRecord), ios::beg);
        if (!file.read(reinterpret_cast<char*>(&rec), sizeof(FileRecord)))
        {
            break;
        }
 
        if (rec.id == id)
        {
            file.close();
            return true;
        }
        pos++;
    }
 
    file.close();
    return false;
}
 
void Create()
{
    int id;
    string name;
    float price, discount;
    char category;
 
    cout << "\nEnter Product ID: ";
    if (!(cin >> id))
    {
        cout << "Invalid ID!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }
 
    if (id <= 0)
    {
        cout << "ID must be positive!" << endl;
        return;
    }
 
    if (idExists(id))
    {
        cout << "Duplicate ID! Record not added." << endl;
        return;
    }
 
    cin.ignore(1000, '\n');
    cout << "Enter Product Name (maximum 49 characters): ";
    getline(cin, name);
 
    if (name.empty() || name.length() > 49)
    {
        cout << "Enter a name from 1 to 49 characters." << endl;
        return;
    }
 
    cout << "Enter Original Price: ";
    if (!(cin >> price) || price <= 0)
    {
        cout << "Invalid price!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }
 
    cout << "Enter Discount (%): ";
    if (!(cin >> discount) || discount < 0 || discount > 100)
    {
        cout << "Discount must be between 0 and 100." << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }
 
    cout << "Enter Category (C/E/F): ";
    cin >> category;
    if (category != 'C' && category != 'E' && category != 'F')
    {
        cout << "Enter C, E or F only." << endl;
        return;
    }
 
    Item temp;
    temp.setData(id, name, price, discount, category);
 
    FileRecord rec;
    rec.id = temp.getId();
    strncpy(rec.name, name.c_str(), sizeof(rec.name) - 1);
    rec.name[sizeof(rec.name) - 1] = '\0';
    rec.price = price;
    rec.discount = discount;
    rec.salePrice = temp.getSalePrice();
    rec.category = category;
 
    // opening file in binary append mode
    fstream file("data/records.dat", ios::binary | ios::out | ios::app);
    if (!file)
    {
        cout << "Could not open data/records.dat. Create the data folder first." << endl;
        return;
    }
 
    file.write(reinterpret_cast<const char*>(&rec), sizeof(FileRecord));
    if (file)
    {
        cout << "Product saved successfully." << endl;
        temp.display();
    }
    else
    {
        cout << "Unable to save record." << endl;
    }
    file.close();
}
 
void Read()
{
    fstream file("data/records.dat", ios::binary | ios::in);
    if (!file)
    {
        cout << "No records file found. Add a product first." << endl;
        return;
    }
 
    FileRecord rec;
    int total = 0;
 
    cout << "\nID\tName\tPrice\tDiscount\tSale Price\tCategory" << endl;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(FileRecord)))
    {
        Item obj(rec.id, rec.name, rec.price, rec.discount, rec.category);
        Item copy(obj);
        copy.display();
        total++;
    }
 
    Item::setCount(total);
    if (total == 0)
    {
        cout << "No product records available." << endl;
    }
    file.close();
}
 
void Search()
{
    int id;
    cout << "\nEnter Product ID to search: ";
    if (!(cin >> id))
    {
        cout << "Invalid ID!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }
 
    fstream file("data/records.dat", ios::binary | ios::in);
    if (!file)
    {
        cout << "No records file found." << endl;
        return;
    }
 
    FileRecord rec;
    long pos = 0;
    int found = 0;
 
    while (true)
    {
        // moving to the required fixed-size record
        file.seekg(pos * sizeof(FileRecord), ios::beg);
        if (!file.read(reinterpret_cast<char*>(&rec), sizeof(FileRecord)))
        {
            break;
        }
 
        if (rec.id == id)
        {
            Item obj(rec.id, rec.name, rec.price, rec.discount, rec.category);
            Item copy(obj);
            cout << "\nID\tName\tPrice\tDiscount\tSale Price\tCategory" << endl;
            copy.display();
            found = 1;
            break;
        }
        pos++;
    }
 
    if (found == 0)
    {
        cout << "Product Not Found!" << endl;
    }
    file.close();
}
 
void Update()
{
    int id;
    cout << "\nEnter Product ID to update: ";
    if (!(cin >> id))
    {
        cout << "Invalid ID!" << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }
 
    fstream file("data/records.dat", ios::binary | ios::in | ios::out);
    if (!file)
    {
        cout << "No records file found." << endl;
        return;
    }
 
    FileRecord rec;
    long pos = 0;
    int found = 0;
 
    while (true)
    {
        file.seekg(pos * sizeof(FileRecord), ios::beg);
        if (!file.read(reinterpret_cast<char*>(&rec), sizeof(FileRecord)))
        {
            break;
        }
 
        if (rec.id == id)
        {
            Item obj(rec.id, rec.name, rec.price, rec.discount, rec.category);
            cout << "\nCurrent record:" << endl;
            obj.display();
 
            cout << "Enter new Discount (%): ";
            float discount;
            if (!(cin >> discount) || discount < 0 || discount > 100)
            {
                cout << "Discount must be between 0 and 100." << endl;
                cin.clear();
                cin.ignore(1000, '\n');
                file.close();
                return;
            }
 
            rec.discount = discount;
            rec.salePrice = rec.price - (rec.price * rec.discount / 100);
 
            file.clear();
            // moving the write pointer to the same record
            file.seekp(pos * sizeof(FileRecord), ios::beg);
            file.write(reinterpret_cast<const char*>(&rec), sizeof(FileRecord));
            file.flush();
 
            if (file)
            {
                cout << "Record updated successfully." << endl;
                Item updated(rec.id, rec.name, rec.price, rec.discount, rec.category);
                updated.display();
            }
            else
            {
                cout << "Unable to update record." << endl;
            }
 
            found = 1;
            break;
        }
        pos++;
    }
 
    if (found == 0)
    {
        cout << "Product Not Found!" << endl;
    }
    file.close();
}
 
int countRecords()
{
    fstream file("data/records.dat", ios::binary | ios::in);
    if (!file)
    {
        return 0;
    }
 
    FileRecord rec;
    int total = 0;
    while (file.read(reinterpret_cast<char*>(&rec), sizeof(FileRecord)))
    {
        total++;
    }
    file.close();
    return total;
}
 
int main()
{
    int choice = -1;
 
    do
    {
        cout << "\nRETAIL PRICE MARKDOWN PLANNER" << endl;
        cout << "1. Add Product" << endl;
        cout << "2. Display All Products" << endl;
        cout << "3. Search Product" << endl;
        cout << "4. Update Discount" << endl;
        cout << "5. Total Records" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter Your Choice: ";
 
        if (!(cin >> choice))
        {
            cout << "Invalid Choice!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
            continue;
        }
 
        switch (choice)
        {
            case 1:
                Create();
                break;
            case 2:
                Read();
                break;
            case 3:
                Search();
                break;
            case 4:
                Update();
                break;
            case 5:
                Item::setCount(countRecords());
                showCount();
                break;
            case 0:
                cout << "Program Exited Successfully!" << endl;
                break;
            default:
                cout << "Invalid Choice!" << endl;
        }
    } while (choice != 0);
 
    return 0;
}
