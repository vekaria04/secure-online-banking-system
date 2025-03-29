import React, { useState } from "react";
import axios from "axios";
import { Container, TextField, Button, Typography, Alert, Box } from "@mui/material";
import { useNavigate } from "react-router-dom";

function ForgotPassword() {
  const [email, setEmail] = useState("");
  const [message, setMessage] = useState("");
  const navigate = useNavigate();

  async function handleSubmit(e) {
    e.preventDefault();
    setMessage("");
    try {
      await axios.post("/forgot-password", { email });
      setMessage("Password reset link sent to your email.");
    } catch (err) {
      setMessage("Failed to send reset link.");
    }
  }

  return (
    <Container maxWidth="sm" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>Forgot Password</Typography>
      {message && <Alert severity="info" sx={{ mb: 2 }}>{message}</Alert>}
      <Box component="form" onSubmit={handleSubmit} sx={{ display: "flex", flexDirection: "column", gap: 2 }}>
        <TextField label="Email" type="email" value={email} onChange={(e) => setEmail(e.target.value)} required />
        <Button variant="contained" type="submit">Reset Password</Button>
        <Button variant="outlined" onClick={() => navigate("/")}>Back to Login</Button>
      </Box>
    </Container>
  );
}

export default ForgotPassword;
