#include "mainwindow.h"  //	Includes our MainWindow class declaration.

#include <QPushButton>//Creates buttons such as Insert, Update and Delete.
#include <QVBoxLayout>//Arranges widgets vertically.
#include <QHBoxLayout>//Arranges widgets horizontally.
#include <QFormLayout>//Arranges labels and input fields in rows.
#include <QSqlQuery>//Executes SQL queries such as SELECT, INSERT, UPDATE and DELETE.
#include <QSqlError>//Retrieves database error information.
#include <QMessageBox>//Displays messages and confirmation dialogs.
#include <QHeaderView>//Controls the appearance of table headers.
#include <QTableWidgetItem>//Stores and displays individual table cells.
#include <QIntValidator>//	Restricts input to valid integers.
#include <QDoubleValidator>//Restricts input to valid decimal numbers.

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)//This is the constructor of the MainWindow class.
{
    QWidget *widget = new QWidget(this);//This widget acts as the container for your application layout.

    // TABLE
    table = new QTableWidget(this);//We use QTableWidget to display database records.

    table->setColumnCount(4);//Your Employee table has four columns:

    table->setHorizontalHeaderLabels(
        {"ID", "Name", "Age", "Salary"}//This assigns names to the four columns.
    );

    table->horizontalHeader()->setStretchLastSection(true);//This makes the last column expand to occupy the remaining horizontal space.

    // INPUT FIELDS
    nameEdit = new QLineEdit(this);
    ageEdit = new QLineEdit(this);//QLineEdit is a Qt widget that allows users to enter text.
    salaryEdit = new QLineEdit(this);

    nameEdit->setPlaceholderText("Enter Name");
    ageEdit->setPlaceholderText("Enter Age");//Placeholder text is the temporary instruction displayed inside an empty input field.
    salaryEdit->setPlaceholderText("Enter Salary");

    ageEdit->setValidator(
        new QIntValidator(1, 120, this)//This restricts age input to integers between 1 and 120.
    );

    salaryEdit->setValidator(
        new QDoubleValidator(0, 10000000, 2, this)//This restricts salary input to decimal numbers between 0 and 10,000,000, with up to two decimal places.
    );

    // BUTTONS
    QPushButton *insertBtn = new QPushButton("Insert");//Adds a new employee.
    QPushButton *updateBtn = new QPushButton("Update");//	Modifies an existing employee.
    QPushButton *deleteBtn = new QPushButton("Delete");//	Removes an employee.
    QPushButton *refreshBtn = new QPushButton("Refresh");//Retrieves the latest database records.
    QPushButton *clearBtn = new QPushButton("Clear");//Clears the input fields.

    // FORM LAYOUT
    QFormLayout *formLayout = new QFormLayout;//QFormLayout arranges labels and input fields in rows.

    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Age:", ageEdit);
    formLayout->addRow("Salary:", salaryEdit);

    // BUTTON LAYOUT
    QHBoxLayout *buttonLayout = new QHBoxLayout;//Creates a horizontal layout.

    buttonLayout->addWidget(insertBtn);
    buttonLayout->addWidget(updateBtn);
    buttonLayout->addWidget(deleteBtn);
    buttonLayout->addWidget(refreshBtn);
    buttonLayout->addWidget(clearBtn);

    // MAIN LAYOUT
    QVBoxLayout *layout = new QVBoxLayout(widget);//Creates a vertical layout inside the main widget.

    layout->addWidget(table);
    layout->addLayout(formLayout);//This arranges the application components vertically.
    layout->addLayout(buttonLayout);

    setCentralWidget(widget);//This sets widget as the central widget of the MainWindow.

    // BUTTON CONNECTIONS
    //A signal is emitted when an event occurs, such as clicking a button.
    //A slot is a function that responds to that event.
    connect(insertBtn, &QPushButton::clicked,
            this, &MainWindow::insertData);//When the Insert button is clicked, execute insertData().

    connect(updateBtn, &QPushButton::clicked,
            this, &MainWindow::updateData);//When Update is clicked, execute updateData().

    connect(deleteBtn, &QPushButton::clicked,
            this, &MainWindow::deleteData);//When Delete is clicked, execute deleteData().

    connect(refreshBtn, &QPushButton::clicked,
            this, &MainWindow::retrieveData);//When Refresh is clicked, retrieve the database records again.

    connect(clearBtn, &QPushButton::clicked,
            this, [this]()
    {
        nameEdit->clear();
        ageEdit->clear();
        salaryEdit->clear();
    });

    // SELECT TABLE ROW
    connect(table, &QTableWidget::currentCellChanged,//currentCellChanged is a signal emitted when the current cell changes.
            this, [this](int currentRow, int, int, int)//We only need currentRow, so the remaining parameters are unnamed.
    {
        if (currentRow < 0)//If no valid row is selected, exit the function.
            return;

        nameEdit->setText(table->item(currentRow, 1)->text());//Retrieves the name from the selected row and displays it in nameEdit.
        ageEdit->setText(table->item(currentRow, 2)->text());//Retrieves the age.
        salaryEdit->setText(table->item(currentRow, 3)->text());//Retrieves the salary.
    });

    // DATABASE
    connectDatabase();//This calls the connectDatabase() function.

    retrieveData();//This retrieves the existing records from the database when the application starts.
}

MainWindow::~MainWindow()
{
    if (db.isOpen())
        db.close();
}


// DATABASE CONNECTION
void MainWindow::connectDatabase()//This defines the database connection function.
{
    db = QSqlDatabase::addDatabase("QSQLITE");//QSqlDatabase manages database connections.

    db.setDatabaseName("employee.db");//This specifies the database filename.

    if (!db.open())//open() attempts to establish the database connection.
                   //The ! operator reverses the result.
                   //"If the database fails to open..."
    {
        QMessageBox::critical(
            this,
            "Database Error",   //Displays the database error message.
            db.lastError().text()
        );

        return;
    }

    QSqlQuery query(db);//Creates a query object associated with the db connection.

    if (!query.exec(      //This executes an SQL command.
        "CREATE TABLE IF NOT EXISTS Employee ("  //Creates a table named Employee if it doesn't already exist.
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT," //Stores employee names.
        "age INTEGER,"  //Stores employee ages.
        "salary REAL)"  //Stores salary values, including decimal values.
    ))
    {
        QMessageBox::critical(
            this,
            "Table Error",
            query.lastError().text() //Retrieves the error if table creation fails.
        );
    }
}


// RETRIEVE DATA
void MainWindow::retrieveData() //This function retrieves records from SQLite and displays them in your QTableWidget.
{
    if (!db.isOpen()) //Checks whether the database is currently open.
        return;

    QSqlQuery query(db); //Creates a query object using the database connection.

    if (!query.exec("SELECT * FROM Employee")) //Retrieve all columns and all records from Employee.
    {
        QMessageBox::warning(
            this,
            "Error",
            query.lastError().text() //Displays the SQL error.
        );

        return;
    }

    table->setRowCount(0);//Removes all currently displayed rows from QTableWidget.

    int row = 0;//This variable tracks the row number in QTableWidget.

    while (query.next()) //query.next() moves the query cursor to the next available record.
    {
        table->insertRow(row);//Creates a new empty row in QTableWidget.

        for (int col = 0; col < 4; col++)//This loop runs four times because we have four columns.
        {
            table->setItem(
                row,
                col,
                new QTableWidgetItem(
                    query.value(col).toString()
                )
            );
        }

        row++;//Moves to the next row position in the GUI table.
    }
}


// INSERT DATA
void MainWindow::insertData()//This function adds a new employee to your SQLite database.
{
    QString name = nameEdit->text();//text() retrieves the text entered by the user.

    bool ageOk;
    bool salaryOk;//These variables will indicate whether the age and salary conversions succeed.

    int age = ageEdit->text().toInt(&ageOk);//toInt() converts the entered QString into an integer.

    double salary = salaryEdit->text().toDouble(&salaryOk);//Converts the salary text into a double.

    if (name.trimmed().isEmpty() || !ageOk || !salaryOk)
    {
        QMessageBox::warning(
            this,
            "Input Error",
            "Please enter valid employee details!"
        );

        return;//Stops execution of the function.
    }

    QSqlQuery query(db);//Creates an SQL query object.

    query.prepare(  //This prepares an SQL INSERT statement.
        "INSERT INTO Employee (name, age, salary) "
        "VALUES (:name, :age, :salary)"
    );

    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":salary", salary);//This assigns the actual values to the placeholders.

    if (!query.exec())//Executes the prepared SQL statement.
    {
        QMessageBox::warning(  //If execution fails, the error is displayed.
            this,
            "Insert Error",
            query.lastError().text()
        );

        return;
    }

    retrieveData();//Retrieves the latest records.

    nameEdit->clear();
    ageEdit->clear();
    salaryEdit->clear();

    QMessageBox::information(  //Displays an insertion success message.
        this,
        "Success",
        "Record inserted successfully!"
    );
}


// UPDATE DATA
void MainWindow::updateData() //This function modifies an existing employee record.
{
    int row = table->currentRow(); //currentRow() returns the currently selected row number.

    if (row < 0) //If no row is selected, currentRow() returns -1.
    {
        QMessageBox::warning(
            this,
            "Warning", //Displays a warning.
            "Select a record first!"
        );

        return;
    }

    int id = table->item(row, 0)->text().toInt();

    QString name = nameEdit->text(); //Gets the modified employee name.

    bool ageOk;
    bool salaryOk; //Declares validation variables.

    int age = ageEdit->text().toInt(&ageOk); //Converts the entered age into an integer.

    double salary = salaryEdit->text().toDouble(&salaryOk); //Converts the entered salary into a double.

    if (name.trimmed().isEmpty() || !ageOk || !salaryOk)  //Checks whether the entered values are valid.
    {
        QMessageBox::warning(
            this,
            "Input Error",
            "Please enter valid employee details!"
        );

        return;
    }

    QSqlQuery query(db); //Creates a query object.

    query.prepare(   //This is the SQL UPDATE statement.
        "UPDATE Employee "  //Specifies the table to update.
        "SET name = :name, age = :age, salary = :salary "  //Specifies the columns and their new values.
        "WHERE id = :id" //Specifies which employee should be updated.
    );

    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":salary", salary);
    query.bindValue(":id", id);  //Assigns the values to their corresponding placeholders.

    if (!query.exec())  //Executes the UPDATE query.
    {
        QMessageBox::warning(
            this,
            "Update Error",
            query.lastError().text()
        );

        return;
    }

    retrieveData();  //Retrieves the latest database values.

    QMessageBox::information(
        this,
        "Success",
        "Record updated successfully!"
    );   //Displays an update success message.
}


// DELETE DATA
void MainWindow::deleteData()  //This function removes an employee record from SQLite.
{
    int row = table->currentRow(); //Gets the selected row number.

    if (row < 0) //Checks whether the user selected a record.
    {
        QMessageBox::warning(
            this,
            "Warning",
            "Select a record first!"
        );

        return;
    }

    QMessageBox::StandardButton reply; //Declares a variable to store the user's response.

    reply = QMessageBox::question(  //Displays a confirmation dialog with two buttons:
        this,
        "Confirm Delete",
        "Are you sure you want to delete this record?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::No)  //Check response
        return;

    int id = table->item(row, 0)->text().toInt(); //Retrieves the ID of the selected employee.

    QSqlQuery query(db);  //Creates the query object.

    query.prepare(
        "DELETE FROM Employee WHERE id = :id"
    ); //This SQL statement deletes the employee whose ID matches the given value.

    query.bindValue(":id", id);  //Assigns the selected employee ID to the placeholder.

    if (!query.exec()) //Executes the delete statement.
    {
        QMessageBox::warning(
            this,
            "Delete Error",
            query.lastError().text()
        );

        return;
    }

    retrieveData(); //Retrieves the latest database records.

    nameEdit->clear();
    ageEdit->clear();
    salaryEdit->clear();

    QMessageBox::information(
        this,
        "Success",
        "Record deleted successfully!"
    );
}
