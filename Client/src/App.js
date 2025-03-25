import React from "react";
import { Routes, Route, Link } from "react-router-dom";
import { AppBar, Toolbar, Typography, Button, Box } from "@mui/material";
import Login from "./pages/Login";
import Register from "./pages/Register";
import Dashboard from "./pages/Dashboard";
import Deposit from "./pages/Deposit";
import Withdraw from "./pages/Withdraw";
import Transfer from "./pages/Transfer";
import TransactionHistory from "./pages/TransactionHistory";
import PasswordUpdate from "./pages/PasswordUpdate";

function App() {
  return (
    <Box>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6" sx={{ flexGrow: 1 }}>
            Online Banking
          </Typography>
          <Button color="inherit" component={Link} to="/">Login</Button>
          <Button color="inherit" component={Link} to="/register">Register</Button>
          <Button color="inherit" component={Link} to="/dashboard">Dashboard</Button>
        </Toolbar>
      </AppBar>
      <Box sx={{ p: 3 }}>
        <Routes>
          <Route path="/" element={<Login />} />
          <Route path="/register" element={<Register />} />
          <Route path="/dashboard" element={<Dashboard />} />
          <Route path="/deposit" element={<Deposit />} />
          <Route path="/withdraw" element={<Withdraw />} />
          <Route path="/transfer" element={<Transfer />} />
          <Route path="/transactions" element={<TransactionHistory />} />
          <Route path="/update-password" element={<PasswordUpdate />} />
        </Routes>
      </Box>
    </Box>
  );
}

export default App;
