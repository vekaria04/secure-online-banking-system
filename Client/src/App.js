import React from "react";
import { Routes, Route, Link, useNavigate } from "react-router-dom";
import { AppBar, Toolbar, Typography, Button, Box } from "@mui/material";
import Login from "./pages/Login";
import Register from "./pages/Register";
import ForgotPassword from "./pages/ForgotPassword";
import Dashboard from "./pages/Dashboard";
import Deposit from "./pages/Deposit";
import Withdraw from "./pages/Withdraw";
import Transfer from "./pages/Transfer";
import TransactionHistory from "./pages/TransactionHistory";
import UpdatePassword from "./pages/UpdatePassword";
import ProfileUpdate from "./pages/ProfileUpdate";

function App() {
  const navigate = useNavigate();
  const token = localStorage.getItem("token");

  function handleLogout() {
    localStorage.removeItem("token");
    localStorage.removeItem("userId");
    localStorage.removeItem("accountId");
    navigate("/");
  }

  return (
    <Box>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6" sx={{ flexGrow: 1 }}>
            Online Banking
          </Typography>
          {!token && (
            <>
              <Button color="inherit" component={Link} to="/">Login</Button>
              <Button color="inherit" component={Link} to="/register">Register</Button>
              {/* <Button color="inherit" component={Link} to="/forgot-password">Forgot Password</Button> */}
            </>
          )}
          {token && (
            <>
              <Button color="inherit" component={Link} to="/dashboard">Dashboard</Button>
              {/* <Button color="inherit" component={Link} to="/profile-update">Profile</Button> */}
              <Button color="inherit" onClick={handleLogout}>Logout</Button>
            </>
          )}
        </Toolbar>
      </AppBar>
      <Box sx={{ p: 3 }}>
        <Routes>
          <Route path="/" element={<Login />} />
          <Route path="/register" element={<Register />} />
          <Route path="/forgot-password" element={<ForgotPassword />} />
          <Route path="/dashboard" element={<Dashboard />} />
          <Route path="/admin" element={<AdminDashboard />} />
          <Route path="/deposit" element={<Deposit />} />
          <Route path="/withdraw" element={<Withdraw />} />
          <Route path="/transfer" element={<Transfer />} />
          <Route path="/transactions" element={<TransactionHistory />} />
          <Route path="/update-password" element={<UpdatePassword />} />
          <Route path="/profile-update" element={<ProfileUpdate />} />
        </Routes>
      </Box>
    </Box>
  );
}

export default App;
