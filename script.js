// Global variables
let inventoryItems = [];
let filteredItems = [];

// DOM elements
const addItemForm = document.getElementById('addItemForm');
const searchInput = document.getElementById('searchInput');
const categoryFilter = document.getElementById('categoryFilter');
const inventoryTableBody = document.getElementById('inventoryTableBody');
const noItemsMessage = document.getElementById('noItemsMessage');
const editModal = document.getElementById('editModal');
const editItemForm = document.getElementById('editItemForm');

// Initialize the application
document.addEventListener('DOMContentLoaded', function() {
    loadInventory();
    setupEventListeners();
});

// Setup event listeners
function setupEventListeners() {
    // Add item form submission
    addItemForm.addEventListener('submit', handleAddItem);
    
    // Search functionality
    searchInput.addEventListener('input', handleSearch);
    
    // Category filter
    categoryFilter.addEventListener('change', handleFilter);
    
    // Edit form submission
    editItemForm.addEventListener('submit', handleEditItem);
    
    // Modal close on outside click
    window.addEventListener('click', function(event) {
        if (event.target === editModal) {
            closeEditModal();
        }
    });
}

// Load inventory from server
async function loadInventory() {
    try {
        showLoading();
        const response = await fetch('/api/items');
        if (!response.ok) {
            throw new Error('Failed to load inventory');
        }
        
        inventoryItems = await response.json();
        filteredItems = [...inventoryItems];
        displayInventory();
        updateStatistics();
        hideLoading();
    } catch (error) {
        console.error('Error loading inventory:', error);
        showNotification('Error loading inventory', 'error');
        hideLoading();
    }
}

// Display inventory in table
function displayInventory() {
    if (filteredItems.length === 0) {
        inventoryTableBody.innerHTML = '';
        noItemsMessage.style.display = 'block';
        return;
    }
    
    noItemsMessage.style.display = 'none';
    
    inventoryTableBody.innerHTML = filteredItems.map(item => `
        <tr>
            <td>${item.id}</td>
            <td><strong>${escapeHtml(item.name)}</strong></td>
            <td>${escapeHtml(item.description || '')}</td>
            <td><span class="category-badge">${escapeHtml(item.category || 'Uncategorized')}</span></td>
            <td class="quantity-cell ${getQuantityClass(item.quantity)}">${item.quantity} ${item.quantityType || ''}</td>
            <td class="price-cell">${currencySymbols[item.currency || selectedCurrency] || ''}${parseFloat(item.price).toFixed(2)}</td>
            <td>${formatDate(item.dateAdded)}</td>
            <td>
                <div class="action-buttons">
                    <button class="btn btn-small btn-success" onclick="editItem('${item.id}')" title="Edit">
                        <i class="fas fa-edit"></i>
                    </button>
                    <button class="btn btn-small btn-danger" onclick="deleteItem('${item.id}')" title="Delete">
                        <i class="fas fa-trash"></i>
                    </button>
                </div>
            </td>
        </tr>
    `).join('');
}

// Handle add item form submission
async function handleAddItem(event) {
    event.preventDefault();
    
    const formData = new FormData(addItemForm);
    const itemData = {
        id: generateSerial(),
        name: formData.get('name').trim(),
        description: formData.get('description').trim(),
        quantity: parseInt(formData.get('quantity')),
        price: parseFloat(formData.get('price')),
        category: formData.get('category').trim(),
        quantityType: formData.get('quantityType'),
        currency: selectedCurrency,
        dateAdded: new Date().toISOString()
    };
    
    // Validation
    if (!itemData.name) {
        showNotification('Item name is required', 'error');
        return;
    }
    
    if (itemData.quantity < 0) {
        showNotification('Quantity must be non-negative', 'error');
        return;
    }
    
    if (itemData.price < 0) {
        showNotification('Price must be non-negative', 'error');
        return;
    }
    
    try {
        // Save locally for demo (remove fetch for local-only)
        inventoryItems.push(itemData);
        filteredItems = [...inventoryItems];
        displayInventory();
        updateStatistics();
        updateCheckout();
        showNotification('Item added successfully', 'success');
        addItemForm.reset();
    } catch (error) {
        console.error('Error adding item:', error);
        showNotification('Error adding item', 'error');
    }
}

// Handle edit item
async function editItem(itemId) {
    const item = inventoryItems.find(item => item.id === itemId);
    if (!item) {
        showNotification('Item not found', 'error');
        return;
    }
    
    // Populate edit form
    document.getElementById('editItemId').value = item.id;
    document.getElementById('editItemName').value = item.name;
    document.getElementById('editItemDescription').value = item.description || '';
    document.getElementById('editItemQuantity').value = item.quantity;
    document.getElementById('editItemPrice').value = item.price;
    document.getElementById('editItemCategory').value = item.category || '';
    editQuantityType.value = item.quantityType || 'KG';
    
    // Show modal
    editModal.style.display = 'block';
}

// Handle edit form submission
async function handleEditItem(event) {
    event.preventDefault();
    
    const formData = new FormData(editItemForm);
    const itemId = document.getElementById('editItemId').value;
    const item = inventoryItems.find(i => i.id === itemId);
    if (!item) {
        showNotification('Item not found', 'error');
        return;
    }
    item.name = formData.get('name').trim();
    item.description = formData.get('description').trim();
    item.quantity = parseInt(formData.get('quantity'));
    item.price = parseFloat(formData.get('price'));
    item.category = formData.get('category').trim();
    item.quantityType = formData.get('quantityType');
    item.currency = selectedCurrency;
    displayInventory();
    updateStatistics();
    updateCheckout();
    showNotification('Item updated successfully', 'success');
    closeEditModal();
}

// Handle delete item
async function deleteItem(itemId) {
    if (!confirm('Are you sure you want to delete this item?')) {
        return;
    }
    
    try {
        const response = await fetch(`/api/items/${itemId}`, {
            method: 'DELETE'
        });
        
        if (!response.ok) {
            throw new Error('Failed to delete item');
        }
        
        showNotification('Item deleted successfully', 'success');
        await loadInventory();
    } catch (error) {
        console.error('Error deleting item:', error);
        showNotification('Error deleting item', 'error');
    }
}

// Handle search
function handleSearch() {
    const searchTerm = searchInput.value.toLowerCase().trim();
    applyFilters();
}

// Handle category filter
function handleFilter() {
    applyFilters();
}

// Apply search and filter
function applyFilters() {
    const searchTerm = searchInput.value.toLowerCase().trim();
    const categoryFilterValue = categoryFilter.value;
    
    filteredItems = inventoryItems.filter(item => {
        const matchesSearch = !searchTerm || 
            item.name.toLowerCase().includes(searchTerm) ||
            (item.description && item.description.toLowerCase().includes(searchTerm)) ||
            (item.category && item.category.toLowerCase().includes(searchTerm));
        
        const matchesCategory = !categoryFilterValue || item.category === categoryFilterValue;
        
        return matchesSearch && matchesCategory;
    });
    
    displayInventory();
}

// Update statistics
function updateStatistics() {
    const totalItems = inventoryItems.length;
    const totalValue = inventoryItems.reduce((sum, item) => sum + (item.quantity * item.price), 0);
    const lowStockItems = inventoryItems.filter(item => item.quantity <= 5).length;
    
    document.getElementById('totalItems').textContent = totalItems;
    document.getElementById('totalValue').textContent = `${currencySymbols[selectedCurrency]}${totalValue.toFixed(2)}`;
    document.getElementById('lowStock').textContent = lowStockItems;
}

// Utility functions
function getQuantityClass(quantity) {
    if (quantity <= 5) return 'quantity-low';
    if (quantity <= 20) return 'quantity-medium';
    return 'quantity-high';
}

function formatDate(dateString) {
    if (!dateString) return 'N/A';
    const date = new Date(dateString);
    return date.toLocaleDateString();
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function clearForm() {
    addItemForm.reset();
}

function closeEditModal() {
    editModal.style.display = 'none';
    editItemForm.reset();
}

function refreshInventory() {
    loadInventory();
}

// Export to CSV
function exportToCSV() {
    if (filteredItems.length === 0) {
        showNotification('No items to export', 'info');
        return;
    }
    
    const headers = ['ID', 'Name', 'Description', 'Category', 'Quantity', 'Price', 'Date Added'];
    const csvContent = [
        headers.join(','),
        ...filteredItems.map(item => [
            item.id,
            `"${item.name}"`,
            `"${item.description || ''}"`,
            `"${item.category || ''}"`,
            item.quantity,
            item.price,
            item.dateAdded
        ].join(','))
    ].join('\n');
    
    const blob = new Blob([csvContent], { type: 'text/csv' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `inventory_${new Date().toISOString().split('T')[0]}.csv`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    window.URL.revokeObjectURL(url);
    
    showNotification('Inventory exported successfully', 'success');
}

// Notification system
function showNotification(message, type = 'info') {
    const notification = document.getElementById('notification');
    notification.textContent = message;
    notification.className = `notification ${type}`;
    notification.classList.add('show');
    
    setTimeout(() => {
        notification.classList.remove('show');
    }, 3000);
}

// Loading states
function showLoading() {
    const refreshBtn = document.querySelector('.table-actions .btn');
    if (refreshBtn) {
        refreshBtn.innerHTML = '<div class="loading"></div> Loading...';
        refreshBtn.disabled = true;
    }
}

function hideLoading() {
    const refreshBtn = document.querySelector('.table-actions .btn');
    if (refreshBtn) {
        refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i> Refresh';
        refreshBtn.disabled = false;
    }
}

// Keyboard shortcuts
document.addEventListener('keydown', function(event) {
    // Ctrl/Cmd + N to focus on add item form
    if ((event.ctrlKey || event.metaKey) && event.key === 'n') {
        event.preventDefault();
        document.getElementById('itemName').focus();
    }
    
    // Ctrl/Cmd + F to focus on search
    if ((event.ctrlKey || event.metaKey) && event.key === 'f') {
        event.preventDefault();
        searchInput.focus();
    }
    
    // Escape to close modal
    if (event.key === 'Escape') {
        closeEditModal();
    }
});

// Auto-refresh every 30 seconds
setInterval(() => {
    loadInventory();
}, 30000); 

// --- Currency, Serial, Quantity Type, Checkout, PDF Invoice Enhancements ---

// Add jsPDF CDN if not present
if (!window.jsPDF) {
    var script = document.createElement('script');
    script.src = 'https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js';
    document.head.appendChild(script);
}

let selectedCurrency = 'PKR';
const currencySymbols = { PKR: '₨', USD: '$' };

// Listen for currency selection
const currencySelect = document.getElementById('currencySelect');
currencySelect.addEventListener('change', function() {
    selectedCurrency = this.value;
    displayInventory();
    updateStatistics();
    updateCheckout();
});

// --- Add Item: Serial Number & Quantity Type ---
function generateSerial() {
    return 'SN-' + Date.now() + '-' + Math.floor(Math.random() * 1000);
}

// --- Checkout Section ---
function updateCheckout() {
    const checkoutList = document.getElementById('checkoutList');
    const checkoutTotal = document.getElementById('checkoutTotal');
    if (!checkoutList || !checkoutTotal) return;
    if (inventoryItems.length === 0) {
        checkoutList.innerHTML = '<p>No items in inventory.</p>';
        checkoutTotal.innerHTML = '';
        return;
    }
    let total = 0;
    checkoutList.innerHTML = `<table style="width:100%;border-collapse:collapse;">
        <tr><th>Serial</th><th>Name</th><th>Qty</th><th>Type</th><th>Price</th><th>Subtotal</th></tr>
        ${inventoryItems.map(item => {
            const subtotal = item.price * item.quantity;
            total += subtotal;
            return `<tr>
                <td>${item.id}</td>
                <td>${escapeHtml(item.name)}</td>
                <td>${item.quantity}</td>
                <td>${item.quantityType || ''}</td>
                <td>${currencySymbols[item.currency || selectedCurrency] || ''}${parseFloat(item.price).toFixed(2)}</td>
                <td>${currencySymbols[item.currency || selectedCurrency] || ''}${subtotal.toFixed(2)}</td>
            </tr>`;
        }).join('')}
    </table>`;
    checkoutTotal.innerHTML = `<strong>Total: ${currencySymbols[selectedCurrency]}${total.toFixed(2)}</strong>`;
}

// --- PDF Invoice Generation ---
const downloadInvoiceBtn = document.getElementById('downloadInvoiceBtn');
downloadInvoiceBtn.addEventListener('click', function() {
    if (!window.jspdf && !window.jsPDF) {
        showNotification('jsPDF not loaded yet. Please try again.', 'error');
        return;
    }
    const doc = new (window.jspdf ? window.jspdf.jsPDF : window.jsPDF.jsPDF)();
    doc.setFontSize(16);
    doc.text('Invoice', 10, 10);
    doc.setFontSize(10);
    let y = 20;
    doc.text(`Date: ${new Date().toLocaleString()}`, 10, y);
    y += 10;
    doc.text(`Currency: ${selectedCurrency}`, 10, y);
    y += 10;
    doc.text('------------------------------------------------------------', 10, y);
    y += 10;
    doc.text('Serial | Name | Qty | Type | Price | Subtotal', 10, y);
    y += 8;
    let total = 0;
    inventoryItems.forEach(item => {
        const subtotal = item.price * item.quantity;
        total += subtotal;
        doc.text(`${item.id} | ${item.name} | ${item.quantity} | ${item.quantityType || ''} | ${currencySymbols[item.currency || selectedCurrency] || ''}${item.price.toFixed(2)} | ${currencySymbols[item.currency || selectedCurrency] || ''}${subtotal.toFixed(2)}`, 10, y);
        y += 8;
        if (y > 270) { doc.addPage(); y = 10; }
    });
    y += 10;
    doc.text('------------------------------------------------------------', 10, y);
    y += 10;
    doc.text(`Total: ${currencySymbols[selectedCurrency]}${total.toFixed(2)}`, 10, y);
    doc.save(`invoice_${new Date().toISOString().split('T')[0]}.pdf`);
});

// --- Update checkout on load and after changes ---
document.addEventListener('DOMContentLoaded', updateCheckout); 