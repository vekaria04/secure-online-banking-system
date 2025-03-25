import React, { useState } from "react";
import axios from "axios";
import { useNavigate } from "react-router-dom";
import { Container, TextField, Button, Typography, Alert, Box } from "@mui/material";

function Login() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const navigate = useNavigate();

  async function handleSubmit(e) {
    e.preventDefault();
    setError("");
    // Hardcoded test login for testing purposes
    if (email === "test@example.com" && password === "password123") {
      localStorage.setItem("userId", "1");
      localStorage.setItem("accountId", "1");
      navigate("/dashboard");
      return;
    }
    try {
      const res = await axios.post("http://localhost:3001/login", {
        email,
        password
      });
      const { userId, accountId } = res.data;
      localStorage.setItem("userId", userId);
      localStorage.setItem("accountId", accountId);
      navigate("/dashboard");
    } catch (err) {
      setError("Login failed");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Login</Typography>
      {error && <Alert severity="error" sx={{ mb: 2 }}>{error}</Alert>}
      <Box component="form" onSubmit={handleSubmit} noValidate sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField 
          label="Email" 
          type="email" 
          value={email} 
          onChange={(e) => setEmail(e.target.value)} 
          required 
        />
        <TextField 
          label="Password" 
          type="password" 
          value={password} 
          onChange={(e) => setPassword(e.target.value)} 
          required 
        />
        <Button variant="contained" type="submit">Login</Button>
      </Box>
    </Container>
  );
}

export default Login;
