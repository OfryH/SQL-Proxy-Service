USE sql_proxy_demo;

-- Customers seed data

INSERT INTO customers (name, email, phone, credit_card)
VALUES 
('Alice Johnson', 'alice@gmail.com', '050-1234567', '4111111111111111'),
('Bob Smith', 'bob@yahoo.com', '052-9876543', '5500000000000004'),
('Charlie Brown', 'charlie@outlook.com', '054-5551234', '340000000000009'),
('Dana White', 'dana@gmail.com', '058-1112223', '6011000000000004');


-- Orders seed data

INSERT INTO orders (customer_id, amount)
VALUES
(1, 120.50),
(1, 89.99),
(2, 45.00),
(3, 310.10),
(4, 15.75);