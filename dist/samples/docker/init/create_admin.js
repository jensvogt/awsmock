// create_admin.js

// Connect to the admin database
db = db.getSiblingDB("admin");

// Create the admin user
db.createUser({
    user: "admin",
    pwd: "admin",        // change this to a strong password
    roles: [
        {role: "userAdminAnyDatabase", db: "admin"},
        {role: "readWriteAnyDatabase", db: "admin"},
        {role: "dbAdminAnyDatabase", db: "admin"}
    ]
});

print("Admin user created successfully");
