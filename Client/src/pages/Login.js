import React, { useState } from "react";
import axios from "axios";
import { useNavigate } from "react-router-dom";
import {
  Container,
  TextField,
  Button,
  Typography,
  Alert,
  Box,
} from "@mui/material";

const BASE = process.env.REACT_APP_API_BASE_URL || "";
function Login() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const navigate = useNavigate();

  async function handleSubmit(e) {
    e.preventDefault();
    setError("");
    try {
      const res = await axios.post(`${BASE}/login`, {
        email,
        password,
      });

      const { token, role } = res.data;
      localStorage.setItem("token", token);

      if (role === "admin") {
        navigate("/admin");
      } else {
        const { userId, accountId } = res.data;
        localStorage.setItem("userId", userId);
        localStorage.setItem("accountId", accountId);
        navigate("/dashboard");
      }
    } catch (err) {
      setError("Login failed");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Login</Typography>
      {error && <Alert severity="error" sx={{ mb: 2 }}>{error}</Alert>}
      <Box
        component="form"
        onSubmit={handleSubmit}
        sx={{ display: "flex", flexDirection: "column", gap: 2 }}
      >
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
