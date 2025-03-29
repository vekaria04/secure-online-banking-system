import React, { useState } from "react";
import axios from "axios";
import { useNavigate } from "react-router-dom";
import { Container, TextField, Button, Typography, Alert, Box } from "@mui/material";
import getBaseUrl from "./utils/getBaseUrl";

const BASE = getBaseUrl();
function Register() {
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const navigate = useNavigate();

  async function handleSubmit(e) {
    e.preventDefault();
    setError("");
    try {
      const res = await axios.post(`${BASE}/register`, {
        name,
        email,
        password
      });
      const { userId, accountId } = res.data;
      localStorage.setItem("userId", userId);
      localStorage.setItem("accountId", accountId);
      navigate("/");
    } catch (err) {
      setError("Registration failed");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Register</Typography>
      {error && <Alert severity="error" sx={{ mb: 2 }}>{error}</Alert>}
      <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField label="Name" value={name} onChange={(e) => setName(e.target.value)} required />
        <TextField label="Email" type="email" value={email} onChange={(e) => setEmail(e.target.value)} required />
        <TextField label="Password" type="password" value={password} onChange={(e) => setPassword(e.target.value)} required />
        <Button variant="contained" type="submit">Register</Button>
      </Box>
    </Container>
  );
}

export default Register;
