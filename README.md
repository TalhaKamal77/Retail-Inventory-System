# Inventory Management System

A simple yet powerful inventory management system built with C++ backend and modern HTML/CSS/JavaScript frontend. This system provides a web-based interface for managing inventory items with features like add, edit, delete, search, and export functionality.

## Features

- **Modern Web Interface**: Beautiful, responsive design with intuitive user experience
- **CRUD Operations**: Create, Read, Update, and Delete inventory items
- **Search & Filter**: Search items by name, description, or category
- **Real-time Statistics**: View total items, total value, and low stock alerts
- **Data Export**: Export inventory data to CSV format
- **Persistent Storage**: Data is stored in JSON format on the local filesystem
- **Cross-platform**: Works on Windows, Linux, and macOS
- **No Database Required**: Simple file-based storage system

## System Requirements

### Windows
- MinGW-w64 or Visual Studio Build Tools
- Windows SDK (for ws2_32 library)

### Linux/macOS
- GCC compiler
- Standard build tools

## Installation & Setup

### 1. Clone or Download the Project
```bash
# If using git
git clone <repository-url>
cd inventory_system

# Or simply download and extract the files
```

### 2. Compile the Server

#### Using Makefile (Recommended)
```bash
# Build the server
make all

# Or simply
make
```

#### Manual Compilation

**Windows:**
```bash
g++ -std=c++11 -Wall -Wextra -O2 server.cpp -o inventory_server.exe -lws2_32
```

**Linux/macOS:**
```bash
g++ -std=c++11 -Wall -Wextra -O2 server.cpp -o inventory_server
```

### 3. Run the Server
```bash
# Using make
make run

# Or directly
./inventory_server.exe  # Windows
./inventory_server      # Linux/macOS
```

### 4. Configure Server Settings
When you run the server, it will prompt you for:
- **Server Address**: Default is `127.0.0.1` (localhost)
- **Server Port**: Default is `8080`
- **Data File Path**: Default is `inventory.json`

### 5. Access the Web Interface
Open your web browser and navigate to:
```
http://localhost:8080
```
(Replace `localhost` with your server address if different)

## Usage

### Adding Items
1. Fill out the "Add New Item" form at the top of the page
2. Enter item details:
   - **Name** (required): Item name
   - **Category**: Select from predefined categories or leave empty
   - **Description**: Optional item description
   - **Quantity** (required): Number of items in stock
   - **Price** (required): Price per item
3. Click "Add Item" to save

### Managing Items
- **Edit**: Click the edit button (pencil icon) next to any item
- **Delete**: Click the delete button (trash icon) next to any item
- **Search**: Use the search box to find items by name, description, or category
- **Filter**: Use the category dropdown to filter items by category

### Exporting Data
- Click the "Export" button to download inventory data as a CSV file
- The file will be named with the current date (e.g., `inventory_2024-01-15.csv`)

### Statistics
The system automatically calculates and displays:
- **Total Items**: Number of unique items in inventory
- **Total Value**: Combined value of all items (quantity × price)
- **Low Stock Items**: Items with quantity ≤ 5

## File Structure

```
inventory_system/
├── server.cpp          # C++ backend server
├── index.html          # Main web interface
├── style.css           # CSS styling
├── script.js           # JavaScript functionality
├── Makefile            # Build configuration
├── README.md           # This file
└── inventory.json      # Data storage (created automatically)
```

## API Endpoints

The server provides the following REST API endpoints:

- `GET /api/items` - Get all inventory items
- `POST /api/items` - Add a new item
- `PUT /api/items/{id}` - Update an existing item
- `DELETE /api/items/{id}` - Delete an item

## Data Storage

Inventory data is stored in JSON format in the specified data file (default: `inventory.json`). The file is automatically created when the first item is added and is updated whenever items are modified.

Example data structure:
```json
[
  {
    "id": "ITEM1",
    "name": "Laptop",
    "description": "High-performance laptop for work",
    "quantity": 10,
    "price": 999.99,
    "category": "Electronics",
    "dateAdded": "2024-01-15"
  }
]
```

## Keyboard Shortcuts

- `Ctrl/Cmd + N`: Focus on the "Add Item" form
- `Ctrl/Cmd + F`: Focus on the search box
- `Escape`: Close the edit modal

## Troubleshooting

### Common Issues

1. **Port Already in Use**
   - Change the port number when prompted
   - Or stop other services using the same port

2. **Permission Denied (Linux/macOS)**
   - Make sure the executable has proper permissions:
   ```bash
   chmod +x inventory_server
   ```

3. **Compilation Errors**
   - Ensure you have the required compiler installed
   - On Windows, make sure MinGW-w64 is properly configured
   - Check that all required libraries are available

4. **Web Interface Not Loading**
   - Verify the server is running and listening on the correct address/port
   - Check firewall settings
   - Ensure all HTML, CSS, and JS files are in the same directory as the server

### Getting Help

If you encounter issues:
1. Check the console output for error messages
2. Verify all files are present and in the correct location
3. Ensure the server has write permissions for the data file directory

## Development

### Adding New Features
- **Backend**: Modify `server.cpp` to add new API endpoints
- **Frontend**: Update HTML, CSS, and JavaScript files as needed
- **Styling**: Modify `style.css` for visual changes

### Building for Production
```bash
# Clean build
make clean
make all

# The executable is ready for distribution
```

## License

This project is open source and available under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

---

**Note**: This is a simple inventory system designed for personal or small business use. For enterprise applications, consider using more robust database systems and additional security measures. 